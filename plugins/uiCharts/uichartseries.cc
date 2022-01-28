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
#include "thread.h"

#include <QAreaSeries>
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


bool uiChartSeries::isVisible() const
{
    return qabstractseries_->isVisible();
}


void uiChartSeries::setVisible( bool yn )
{
    qabstractseries_->setVisible( yn );
}


QAbstractSeries* uiChartSeries::getQSeries()
{
    return qabstractseries_;
}


// uiXYChartSeries
uiXYChartSeries::uiXYChartSeries( QXYSeries* series )
    : uiChartSeries(series)
    , pressed(this)
    , released(this)
    , clicked(this)
    , doubleClicked(this)
    , hoverOn(this)
    , hoverOff(this)
    , callouttxt_("X: %1\nY:%2")
{
    qxyseries_ = dynamic_cast<QXYSeries*>(qabstractseries_);
}


uiXYChartSeries::~uiXYChartSeries()
{
    detachAllNotifiers();
    deleteAndZeroPtr( callout_ );
    deleteAndZeroPtr( msghandler_ );
}


void uiXYChartSeries::initCallBacks()
{
    msghandler_ = new i_xySeriesMsgHandler( this, qxyseries_ );
    mAttachCB( this->hoverOn, uiXYChartSeries::showCallout );
    mAttachCB( this->hoverOff, uiXYChartSeries::hideCallout );
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
    if ( !mIsUdf(x) && !mIsUdf(y) )
	qxyseries_->append( qreal(x), qreal(y) );
}


void uiXYChartSeries::append( int num, float* xarr, float* yarr )
{

    for ( float *px=xarr, *py=yarr; px<xarr+num; px++, py++ )
    {
	if ( mIsUdf(*px) || mIsUdf(*py) )
	    continue;

	qxyseries_->append( qreal(*px), qreal(*py) );
    }
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


void uiXYChartSeries::replace( int sz, const float* xarr, const float* yarr,
			       float xshift, float yshift )
{
    QVector<QPointF> points( sz );
    for ( int idx=0; idx<sz; idx++ )
    {
	if ( mIsUdf(xarr[idx]) || mIsUdf(yarr[idx]) )
	    continue;

	points[idx] = QPointF( xarr[idx]+xshift, yarr[idx]+yshift );
    }

    qxyseries_->replace( points );
}


bool uiXYChartSeries::replace_x( int sz, const float* xarr, float xshift )
{
    if ( sz != size() )
	return false;

    QVector<QPointF> points = qxyseries_->pointsVector();
    for ( int idx=0; idx<sz; idx++ )
	points[idx].setX( xarr[idx]+xshift );

    qxyseries_->replace( points );
    return true;
}


bool uiXYChartSeries::replace_y( int sz, const float* yarr, float yshift )
{
    if ( sz != size() )
	return false;

    QVector<QPointF> points = qxyseries_->pointsVector();
    for ( int idx=0; idx<sz; idx++ )
	points[idx].setY( yarr[idx]+yshift );

    qxyseries_->replace( points );
    return true;
}


void uiXYChartSeries::setAll_Y( float newy )
{
    QVector<QPointF> points = qxyseries_->pointsVector();
    for ( int idx=0; idx<size(); idx++ )
	points[idx].setY( newy );

    qxyseries_->replace( points );
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


void uiLineSeries::copyPoints( const uiLineSeries& other )
{
    qlineseries_->replace( other.qlineseries_->pointsVector() );
}


bool uiLineSeries::pointsVisible() const
{
    return qlineseries_->pointsVisible();
}


void uiLineSeries::setPointsVisible( bool yn )
{
    qlineseries_->setPointsVisible( yn );
}


bool uiLineSeries::lineVisible() const
{
    return lineStyle().type_ != OD::LineStyle::None;
}


void uiLineSeries::setLineVisible( bool yn )
{
    if ( yn && !lineVisible() )
    {
	OD::LineStyle ls = lineStyle();
	ls.type_ = savedlinetype_;
	setLineStyle( ls );
    }
    else if ( !yn && lineVisible() )
    {
	OD::LineStyle ls = lineStyle();
	savedlinetype_ = ls.type_;
	ls.type_ = OD::LineStyle::None;
	setLineStyle( ls );
    }
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
    toQColor( qcol, color, true );
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


void uiScatterSeries::copyPoints( const uiScatterSeries& other )
{
    qscatterseries_->replace( other.qscatterseries_->pointsVector() );
}


QScatterSeries* uiScatterSeries::getQScatterSeries()
{
    return qscatterseries_;
}


// uiAreaSeries
uiAreaSeries::uiAreaSeries( uiLineSeries* upper, uiLineSeries* lower )
    : uiChartSeries(new QAreaSeries(upper->getQLineSeries(),
				    lower ? lower->getQLineSeries() : nullptr))
    , upperseries_(upper)
    , lowerseries_(lower)
{
    qareaseries_ = dynamic_cast<QAreaSeries*>(qabstractseries_);
}


uiAreaSeries::~uiAreaSeries()
{
}


OD::Color uiAreaSeries::color() const
{
    return fromQColor( qareaseries_->color() );
}


OD::LineStyle uiAreaSeries::borderStyle() const
{
    return fromQPen( qareaseries_->pen() );
}


void uiAreaSeries::setColor( OD::Color color )
{
    QColor qcol;
    toQColor( qcol, color );
    qareaseries_->setColor( qcol );
}


void uiAreaSeries::setBorderStyle( const OD::LineStyle& ls, bool usetransp )
{
    QPen qpen;
    toQPen( qpen, ls, usetransp, true );
    qareaseries_->setPen( qpen );
}


uiLineSeries* uiAreaSeries::upperSeries() const
{
    return upperseries_;
}


uiLineSeries* uiAreaSeries::lowerSeries() const
{
    return lowerseries_;
}


void uiAreaSeries::setUpperSeries( uiLineSeries* series )
{
    upperseries_ = series;
    qareaseries_->setUpperSeries( series->getQLineSeries() );
}


void uiAreaSeries::setLowerSeries( uiLineSeries* series )
{
    lowerseries_ = series;
    qareaseries_->setLowerSeries( series->getQLineSeries() );
}


bool uiAreaSeries::pointsVisible() const
{
    return qareaseries_->pointsVisible();
}


void uiAreaSeries::setPointsVisible( bool yn )
{
    qareaseries_->setPointsVisible( yn );
}


bool uiAreaSeries::linesVisible() const
{
    return borderStyle().type_ != OD::LineStyle::None;
}


void uiAreaSeries::setLinesVisible( bool yn )
{
    if ( yn && !linesVisible() )
    {
	OD::LineStyle ls = borderStyle();
	ls.type_ = savedlinetype_;
	setBorderStyle( ls );
    }
    else if ( !yn && linesVisible() )
    {
	OD::LineStyle ls = borderStyle();
	savedlinetype_ = ls.type_;
	ls.type_ = OD::LineStyle::None;
	setBorderStyle( ls );
    }
}


void uiAreaSeries::initCallBacks()
{}


QAreaSeries* uiAreaSeries::getQAreaSeries()
{
    return qareaseries_;
}
