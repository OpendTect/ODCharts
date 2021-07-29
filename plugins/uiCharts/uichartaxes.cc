/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		May 2021
________________________________________________________________________

-*/

#include "uichartaxes.h"
#include "i_qchartaxes.h"
#include "chartutils.h"

#include <QLogValueAxis>
#include <QValueAxis>

using namespace QtCharts;




uiChartAxis::uiChartAxis( QAbstractAxis* axis )
    : qabstractaxis_(axis)
{
}


uiChartAxis::~uiChartAxis()
{
}


void uiChartAxis::setTitleText( const char* title )
{
    qabstractaxis_->setTitleText( title );
}


BufferString uiChartAxis::titleText() const
{
    return BufferString( qabstractaxis_->titleText() );
}


void uiChartAxis::setLineStyle( const OD::LineStyle& ls, bool usetransp )
{
    QPen qpen;
    toQPen( qpen, ls, usetransp, true );
    qabstractaxis_->setLinePen( qpen );
}


void uiChartAxis::setGridStyle( const OD::LineStyle& ls, bool usetransp )
{
    QPen qpen;
    toQPen( qpen, ls, usetransp, true );
    qabstractaxis_->setGridLinePen( qpen );
}


void uiChartAxis::setMinorGridStyle( const OD::LineStyle& ls, bool usetransp )
{
    QPen qpen;
    toQPen( qpen, ls, usetransp, true );
    qabstractaxis_->setMinorGridLinePen( qpen );
}


OD::LineStyle uiChartAxis::lineStyle() const
{
    return fromQPen( qabstractaxis_->linePen() );
}


OD::LineStyle uiChartAxis::getGridStyle() const
{
    return fromQPen( qabstractaxis_->gridLinePen() );
}


OD::LineStyle uiChartAxis::getMinorGridStyle() const
{
    return fromQPen( qabstractaxis_->minorGridLinePen() );
}


bool uiChartAxis::gridVisible() const
{
    return qabstractaxis_->isGridLineVisible();
}


void uiChartAxis::setGridLineVisible( bool yn )
{
    qabstractaxis_->setGridLineVisible( yn );
}


bool uiChartAxis::minorGridVisible() const
{
    return qabstractaxis_->isMinorGridLineVisible();
}


void uiChartAxis::setMinorGridLineVisible( bool yn )
{
    qabstractaxis_->setMinorGridLineVisible( yn );
}


OD::Orientation uiChartAxis::orientation() const
{
    return qabstractaxis_->orientation()==Qt::Vertical ? OD::Vertical
						       : OD::Horizontal;
}


void uiChartAxis::setRange( const Interval<float>& range )
{
    if ( range.isRev() )
    {
	setReverse( true );
	setRange( range.stop, range.start );
    }
    else
    {
	setReverse( false );
	setRange( range.start, range.stop );
    }
}


void uiChartAxis::setRange( float min, float max )
{
    qabstractaxis_->setRange( min, max );
}


void uiChartAxis::setReverse( bool yn )
{
    qabstractaxis_->setReverse( yn );
}


Interval<float> uiChartAxis::range() const
{
    auto* qvalueaxis = dynamic_cast<QValueAxis*>(qabstractaxis_);
    auto* qlogvalueaxis = dynamic_cast<QLogValueAxis*>(qabstractaxis_);
    float fmin, fmax;
    if ( qvalueaxis )
    {
	fmin = qvalueaxis->min();
	fmax = qvalueaxis->max();
    }
    else if ( qlogvalueaxis )
    {
	fmin = qlogvalueaxis->min();
	fmax = qlogvalueaxis->max();
    }
    else
	return Interval<float>::udf();

    if ( reversed() )
	return Interval<float>( fmax, fmin );
    else
	return Interval<float>( fmin, fmax );
}


bool uiChartAxis::reversed() const
{
    return qabstractaxis_->isReverse();
}


void uiChartAxis::setMinorTickCount( int count )
{
    auto* qvalueaxis = dynamic_cast<QValueAxis*>(qabstractaxis_);
    auto* qlogvalueaxis = dynamic_cast<QLogValueAxis*>(qabstractaxis_);
    if ( qvalueaxis )
	qvalueaxis->setMinorTickCount( count );
    if ( qlogvalueaxis )
	qlogvalueaxis->setMinorTickCount( count );
}


int uiChartAxis::getMinorTickCount() const
{
    auto* qvalueaxis = dynamic_cast<QValueAxis*>(qabstractaxis_);
    auto* qlogvalueaxis = dynamic_cast<QLogValueAxis*>(qabstractaxis_);
    if ( qvalueaxis )
	return qvalueaxis->minorTickCount();
    if ( qlogvalueaxis )
	return qlogvalueaxis->minorTickCount();

    return mUdf(int);
}


void uiChartAxis::setTickCount( int count )
{
    auto* qvalueaxis = dynamic_cast<QValueAxis*>(qabstractaxis_);
    if ( qvalueaxis )
	qvalueaxis->setTickCount( count );
}


int uiChartAxis::getTickCount() const
{
    auto* qvalueaxis = dynamic_cast<QValueAxis*>(qabstractaxis_);
    auto* qlogvalueaxis = dynamic_cast<QLogValueAxis*>(qabstractaxis_);
    if ( qvalueaxis )
	return qvalueaxis->tickCount();
    if ( qlogvalueaxis )
	return qlogvalueaxis->tickCount();

    return mUdf(int);
}


void uiChartAxis::setTickInterval( float step )
{
    auto* qvalueaxis = dynamic_cast<QValueAxis*>(qabstractaxis_);
    if ( qvalueaxis )
	qvalueaxis->setTickInterval( step );
}


float uiChartAxis::getTickInterval() const
{
    auto* qvalueaxis = dynamic_cast<QValueAxis*>(qabstractaxis_);
    if ( qvalueaxis )
	return qvalueaxis->tickInterval();

    return mUdf(float);
}

uiChartAxis::AxisType uiChartAxis::type() const
{
    return AxisType(qabstractaxis_->type());
}


QAbstractAxis* uiChartAxis::getQAxis()
{
    return qabstractaxis_;
}


// uiValueAxis
uiValueAxis::uiValueAxis()
    : uiChartAxis(new QValueAxis)
    , axislimits_(Interval<float>::udf())
{
    qvalueaxis_ = dynamic_cast<QValueAxis*>(qabstractaxis_);
    msghandler_ = new i_valueAxisMsgHandler( this, qvalueaxis_ );
}


uiValueAxis::~uiValueAxis()
{
    delete msghandler_;
}


void uiValueAxis::setLabelFormat( const char* format )
{
    qvalueaxis_->setLabelFormat( format );
}


BufferString uiValueAxis::labelFormat() const
{
    return BufferString( qvalueaxis_->labelFormat() );
}


void uiValueAxis::setTickType( TickType type )
{
    QValueAxis::TickType qtype = type==TicksDynamic ? QValueAxis::TicksDynamic
						    : QValueAxis::TicksFixed;
    qvalueaxis_->setTickType( qtype );
}


void uiValueAxis::setTickInterval( float intv )
{
    qvalueaxis_->setTickInterval( qreal(intv) );
}


void uiValueAxis::setAxisLimits( const Interval<float>& range, bool include )
{
    if ( include )
       axislimits_.include( range );
    else
       axislimits_ = range;

    setRange( axislimits_.start, axislimits_.stop );
}


void uiValueAxis::setAxisLimits( float min, float max, bool include )
{
    const Interval<float> range( min, max );
    setAxisLimits( range, include );
}


void uiValueAxis::snapRange( float min, float max )
{
    const Interval<float> range( min, max );
    if ( axislimits_.isUdf() || axislimits_.includes(range) )
	return;

    const float width = range.width();
    if ( min<axislimits_.start )
    {
	min = axislimits_.start;
	max = min + width;
    }

    if ( max>axislimits_.stop )
    {
	max = axislimits_.stop;
	min = max-width<axislimits_.start ? axislimits_.start : max-width;
    }

    setRange( min, max );
}


Interval<float> uiValueAxis::getAxisLimits()
{
    return axislimits_;
}


// uiLogValueAxis
uiLogValueAxis::uiLogValueAxis()
    : uiChartAxis(*this)
{
}


uiLogValueAxis::~uiLogValueAxis()
{
}


void uiLogValueAxis::setBase( float baseval )
{
    qlogvalueaxis_->setBase( qreal(baseval) );
}


void uiLogValueAxis::setLabelFormat( const char* format )
{
    qlogvalueaxis_->setLabelFormat( format );
}


BufferString uiLogValueAxis::labelFormat() const
{
    return BufferString( qlogvalueaxis_->labelFormat() );
}
