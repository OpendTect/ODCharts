/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		November 2020
________________________________________________________________________

-*/

#include "uichartseries.h"
#include "i_qchartseries.h"
#include "uicallout.h"
#include "uichartaxes.h"
#include "uimain.h"
#include "uimainwin.h"
#include "chartutils.h"

#include <QLineSeries>
#include <QScatterSeries>

using namespace QtCharts;

uiChartSeries::uiChartSeries( QAbstractSeries* series )
    : qabstractseries_(series)
{
}


uiChartSeries::~uiChartSeries()
{
}


void uiChartSeries::attachAxis( uiChartAxis* axis )
{
    qabstractseries_->attachAxis( axis->getQAxis() );
}


void uiChartSeries::setName( const char* nm )
{
    qabstractseries_->setName( nm );
}


BufferString uiChartSeries::name() const
{
    BufferString str( qabstractseries_->name() );
    return str;
}


QAbstractSeries* uiChartSeries::getQSeries()
{
    return qabstractseries_;
}


// uiXYChartSeries
uiXYChartSeries::uiXYChartSeries( QXYSeries* series )
    : uiChartSeries(series)
    , clicked(this)
    , doubleClicked(this)
    , hoverOn(this)
    , hoverOff(this)
    , callouttxt_("X: %1\nY:%2")
{
    qxyseries_ = dynamic_cast<QXYSeries*>(qabstractseries_);
    msghandler_ = new i_xySeriesMsgHandler( this, qxyseries_ );
    mAttachCB( this->hoverOn, uiXYChartSeries::showCallout );
    mAttachCB( this->hoverOff, uiXYChartSeries::hideCallout );
}


uiXYChartSeries::~uiXYChartSeries()
{
    detachAllNotifiers();
    delete callout_;
    delete msghandler_;
}


void uiXYChartSeries::clear()
{
    qxyseries_->clear();
}


bool uiXYChartSeries::validIdx( int idx ) const
{
    return idx>=0 && idx<size();
}

void uiXYChartSeries::append( float x, float y )
{
    qxyseries_->append( qreal(x), qreal(y) );
}


void uiXYChartSeries::append( int num, float* xarr, float* yarr )
{

    for ( float *px=xarr, *py=yarr; px<xarr+num; px++, py++ )
	qxyseries_->append( qreal(*px), qreal(*py) );
}


float uiXYChartSeries::x( int idx ) const
{
    if ( !validIdx(idx) )
	return mUdf(float);

    return qxyseries_->at( idx ).x();
}


float uiXYChartSeries::y( int idx ) const
{
    if ( !validIdx(idx) )
	return mUdf(float);

    return qxyseries_->at( idx ).y();
}


int uiXYChartSeries::size() const
{
    return qxyseries_->count();
}


bool uiXYChartSeries::isEmpty() const
{
    return size() == 0;
}


void uiXYChartSeries::setCalloutTxt( const char* txt, int nrdecx, int nrdecy )
{
    callouttxt_ = txt;
    nrdecx_ = nrdecx;
    nrdecy_ = nrdecy;
}


void uiXYChartSeries::setPointLabelsVisible( bool yn )
{
    qxyseries_->setPointLabelsVisible( yn );
}


void uiXYChartSeries::setPointLabelsFormat( const char* fmt )
{
    qxyseries_->setPointLabelsFormat( fmt );
}


void uiXYChartSeries::showCallout( CallBacker* cb )
{
    if ( !callout_ )
	callout_ = new uiCallout( this );

    mCBCapsuleUnpack(const Geom::PointF&,pos,cb);
    if ( callouttxt_.find("%1") && callouttxt_.find("%2") )
	callout_->setText(
		tr(callouttxt_).arg(pos.x,nrdecx_).arg(pos.y,nrdecy_) );
    else if ( callouttxt_.find("%1") )
	callout_->setText( tr(callouttxt_).arg(pos.x,nrdecx_) );
    else if ( callouttxt_.find("%2") )
	callout_->setText( tr(callouttxt_).arg(pos.y,nrdecy_) );
    else
	callout_->setText( tr(callouttxt_) );

    callout_->setAnchor( pos, this );
    callout_->setZValue( 11 );
    callout_->update();
    callout_->show();
}


void uiXYChartSeries::hideCallout( CallBacker* )
{
    if ( callout_ )
	callout_->hide();
}


// uiLineSeries
uiLineSeries::uiLineSeries()
    : uiXYChartSeries(new QLineSeries)
{
    qlineseries_ = dynamic_cast<QLineSeries*>(qxyseries_);
}


uiLineSeries::~uiLineSeries()
{
}


void uiLineSeries::setLineStyle( const OD::LineStyle& ls, bool usetransp )
{
    QPen qpen;
    toQPen( qpen, ls, usetransp, true );
    qlineseries_->setPen( qpen );
}


OD::LineStyle uiLineSeries::lineStyle() const
{
    return fromQPen( qlineseries_->pen() );
}


QLineSeries* uiLineSeries::getQLineSeries()
{
    return qlineseries_;
}


// uiScatterSeries
uiScatterSeries::uiScatterSeries()
    : uiXYChartSeries(new QScatterSeries)
{
    qscatterseries_ = dynamic_cast<QScatterSeries*>(qxyseries_);
}


uiScatterSeries::~uiScatterSeries()
{
}


uiScatterSeries::MarkerShape uiScatterSeries::shape() const
{
    return sCast(uiScatterSeries::MarkerShape,qscatterseries_->markerShape());
}


OD::Color uiScatterSeries::color() const
{
    return fromQColor( qscatterseries_->color() );
}


OD::Color uiScatterSeries::borderColor() const
{
    return fromQColor( qscatterseries_->borderColor() );
}


float uiScatterSeries::markerSize() const
{
    return qscatterseries_->markerSize();
}


void uiScatterSeries::setShape( uiScatterSeries::MarkerShape shp )
{
    qscatterseries_->setMarkerShape( sCast(QScatterSeries::MarkerShape,shp) );
}


void uiScatterSeries::setColor( OD::Color color )
{
    QColor qcol;
    toQColor( qcol, color );
    qscatterseries_->setColor( qcol );
}


void uiScatterSeries::setBorderColor( OD::Color color )
{
    QColor qcol;
    toQColor( qcol, color );
    qscatterseries_->setBorderColor( qcol );
}


void uiScatterSeries::setMarkerSize( float size )
{
    qscatterseries_->setMarkerSize( qreal(size) );
}
