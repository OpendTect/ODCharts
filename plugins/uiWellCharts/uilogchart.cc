/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		May 2021
________________________________________________________________________

-*/

#include "uilogchart.h"

#include "uichartaxes.h"
#include "uichartseries.h"
#include "uistring.h"

#include "draw.h"
#include "mnemonics.h"
#include "multiid.h"
#include "welldata.h"
#include "welllog.h"
#include "wellman.h"

uiLogChart::uiLogChart( ZType ztype, Scale scale )
    : ztype_(ztype)
    , scale_(scale)
{
    makeZaxis();
}


uiLogChart::~uiLogChart()
{}


void uiLogChart::addLogCurve( const MultiID& wellid, const char* lognm )
{
    Well::LoadReqs lreq( Well::LogInfos );
    RefMan<Well::Data> wd = Well::MGR().get( wellid, lreq );
    if ( !wd || !wd->getLog(lognm) )
	return;

    const Well::Log& log = *wd->getLog( lognm );
    int idx = MNC().indexOf( log.mnemLabel() );
    OD::LineStyle lstyle;
    float min, max;
    bool reverse;
    if ( idx>=0 )
    {
	const Mnemonic::DispDefs& disp = MNC().get( idx )->disp_;
	min = disp.range_.start;
	max = disp.range_.stop;
	lstyle.color_ = disp.color_;
	reverse = false;
    }
    else
    {
	min = log.valueRange().start;
	max = log.valueRange().stop;
	reverse = false;
    }

    addLogCurve( log, lstyle, min, max, reverse );
}


void uiLogChart::addLogCurve( const MultiID& wellid, const char* lognm,
			      const OD::LineStyle& lstyle )
{
    Well::LoadReqs lreq( Well::LogInfos );
    RefMan<Well::Data> wd = Well::MGR().get( wellid, lreq );
    if ( !wd || !wd->getLog(lognm) )
	return;

    const Well::Log& log = *wd->getLog( lognm );
    addLogCurve( log, lstyle, log.valueRange().start, log.valueRange().stop,
		 false );
}


void uiLogChart::addLogCurve( const Well::Log& log, const OD::LineStyle& lstyle,
			      float min, float max, bool reverse )
{
    BufferString logtitle( log.name() );
    logtitle.add(" (").add(log.unitMeasLabel()).add(")");
    uiChartAxis* logaxis = makeLogAxis( logtitle, min, max, reverse );
    logaxis->setLineStyle( lstyle );
    addAxis( logaxis, OD::Top );
    zaxis_->setAxisLimits( log.dahRange() );
    const BufferString callouttxt( "Depth: %2\n", log.name(), ": %1" );

    uiLineSeries* series = new uiLineSeries();
    for (int idx=0; idx<log.size(); idx++)
    {
	const float logval = log.value( idx );
	const float dah = log.dah( idx ); //TODO handle other ZTypes
	if ( !mIsUdf(logval) )
	    series->append( logval, dah );
	else
	{
	    if ( !series->isEmpty() )
	    {
		series->setLineStyle( lstyle );
		addSeries( series );
		series->attachAxis( zaxis_ );
		series->attachAxis( logaxis );
		series->setCalloutTxt( callouttxt );
		series = new uiLineSeries();
	    }
	}
    }

    if ( !series->isEmpty() )
    {
	series->setLineStyle( lstyle );
	addSeries( series );
	series->attachAxis( zaxis_ );
	series->attachAxis( logaxis );
	series->setCalloutTxt( callouttxt );
    }
    else
	delete series;
}


void uiLogChart::removeLogCurve( const MultiID& wellid, const char* lognm )
{
}


void uiLogChart::removeAllCurves()
{
    removeAllSeries();
    removeAllAxes( OD::Horizontal );
}


uiValueAxis* uiLogChart::getZAxis() const
{
    return zaxis_;
}


void uiLogChart::setZRange( float minz, float maxz )
{
    zaxis_->setRange( minz, maxz );
}


void uiLogChart::setZRange( const Interval<float>& zrange )
{
    setZRange( zrange.start, zrange.stop );
}


void uiLogChart::makeZaxis()
{
    zaxis_ = new uiValueAxis;
    zaxis_->setTickType( uiValueAxis::TicksDynamic );
    zaxis_->setTickInterval( ztype_==TWT ? 0.1 : 100 );
    zaxis_->setMinorTickCount( 4 );
    zaxis_->setLabelFormat( "%d" );
    zaxis_->setRange( 0, 1000 );
    zaxis_->setReverse( true );
    addAxis( zaxis_, OD::Left );
}


uiChartAxis* uiLogChart::makeLogAxis( const BufferString& logtitle, float min,
				      float max, bool reverse )
{
    uiChartAxis* axis = nullptr;
    if ( scale_==Linear )
    {
	uiValueAxis* vaxis = new uiValueAxis;
	vaxis->setTickType( uiValueAxis::TicksFixed );
	vaxis->setTickCount( 2 );
	vaxis->setMinorTickCount( 9 );
	vaxis->setLabelFormat( "%d" );
	vaxis->setRange( min, max );
	vaxis->setReverse( reverse );
	axis = vaxis;
    }
    else if ( scale_==Log10 )
    {
	uiLogValueAxis* vaxis = new uiLogValueAxis;
	vaxis->setBase( 10 );
	vaxis->setMinorTickCount( 9 ); //TODO check
	vaxis->setRange( min, max ); //TODO check min and max
	vaxis->setReverse( reverse );
	axis = vaxis;
    }

    if ( axis )
	axis->setTitleText( logtitle );

    return axis;
}
