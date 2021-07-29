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


void uiXYChartSeries::add( float x, float y )
{
    qxyseries_->append( qreal(x), qreal(y) );
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


void uiXYChartSeries::showCallout( CallBacker* cb )
{
    if ( !callout_ )
	callout_ = new uiCallout( this );

    mCBCapsuleUnpack(const Geom::PointF&,pos,cb);
    callout_->setText(tr(callouttxt_).arg(pos.x,nrdecx_).arg(pos.y,nrdecy_));
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


void uiLineSeries::append( float x, float y )
{
    qlineseries_->append( qreal(x), qreal(y) );
}


void uiLineSeries::append( int num, float* xarr, float* yarr )
{

    for ( float *px=xarr, *py=yarr; px<xarr+num; px++, py++ )
	append( *px, *py );
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


// uiScatterSeries
uiScatterSeries::uiScatterSeries()
    : uiXYChartSeries(new QScatterSeries)
{
    qscatterseries_ = dynamic_cast<QScatterSeries*>(qxyseries_);
}


uiScatterSeries::~uiScatterSeries()
{
}
