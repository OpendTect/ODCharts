/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		August 2020
________________________________________________________________________

-*/


#include "logcurve.h"
#include "chartutils.h"

#include "uichartaxes.h"
#include "uilogchart.h"

#include "draw.h"
#include "mnemonics.h"
#include "multiid.h"
#include "welldata.h"
#include "welllog.h"
#include "wellman.h"

LogCurve::LogCurve()
    : wellid_(MultiID::udf())
    , logname_(BufferString::empty())
{}


LogCurve::LogCurve( const MultiID& wellid, const char* lognm )
    : wellid_(wellid)
    , logname_(lognm)
{
    initLog();
}


LogCurve::~LogCurve()
{
    deepErase( series_ );
}


bool LogCurve::initLog()
{
    Well::LoadReqs lreq( Well::LogInfos );
    RefMan<Well::Data> wd = Well::MGR().get( wellid_, lreq );
    if ( !wd || !wd->getLog(logname_) )
	return false;

    const Well::Log& log = *wd->getLog( logname_ );
    wellname_ = wd->name();
    uomlbl_ = log.unitMeasLabel();
    mnemlbl_ = log.mnemLabel();
    dahrange_ = log.dahRange();
    valrange_ = log.valueRange();
    disprange_.setUdf();
    addLog( log );
    return true;
}


void LogCurve::addTo( uiLogChart& logchart )
{
    int idx = MNC().indexOf( mnemlbl_ );
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
	min = valrange_.start;
	max = valrange_.stop;
	reverse = false;
    }
    if ( !disprange_.isUdf() )
    {
	min = disprange_.isRev() ? disprange_.stop : disprange_.start;
	max = disprange_.isRev() ? disprange_.start : disprange_.stop;
	reverse = disprange_.isRev();
    }
    addTo( logchart, lstyle, min, max, reverse );
}


void LogCurve::addTo( uiLogChart& logchart, const IOPar& iop )
{
    usePar( iop );
    float min = disprange_.isRev() ? disprange_.stop : disprange_.start;
    float max = disprange_.isRev() ? disprange_.start : disprange_.stop;
    bool reverse = disprange_.isRev();
    addTo( logchart, linestyle_, min, max, reverse );
}

void LogCurve::addTo( uiLogChart& logchart, const OD::LineStyle& lstyle )
{

    addTo( logchart, lstyle, valrange_.start, valrange_.stop, false );
}


void LogCurve::addTo( uiLogChart& logchart, const OD::LineStyle& lstyle,
		      float min, float max, bool reverse )
{
    BufferString logtitle( logname_ );
    logtitle.add(" (").add( uomlbl_ ).add(")");
    axis_ = logchart.makeLogAxis( logtitle, min, max, reverse );
    axis_->setLineStyle( lstyle );
    disprange_ = axis_->range();
    logchart.addAxis( axis_, OD::Top );
    logchart.getZAxis()->setAxisLimits( dahrange_ );
    BufferString callouttxt( wellname_, "\nDepth: %2\n", logname_ );
    callouttxt.add( ": %1" );
    linestyle_ = lstyle;
    for ( auto* series : series_ )
    {
	series->setLineStyle( lstyle );
	logchart.addSeries( series );
	series->attachAxis( logchart.getZAxis() );
	series->attachAxis( axis_ );
	series->setCalloutTxt( callouttxt );
    }
}


void LogCurve::addLog( const Well::Log& log )
{
    series_.setEmpty();
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
		series_ += series;
		series = new uiLineSeries();
	    }
	}
    }

    if ( !series->isEmpty() )
	series_ += series;
    else
	delete series;
}


void LogCurve::setLineStyle(const OD::LineStyle& ls, bool usetransp)
{
    linestyle_ = ls;
    for ( auto* series : series_ )
    {
	series->setLineStyle( linestyle_, usetransp );
    }
    if (axis_ )
	axis_->setLineStyle( linestyle_ );
}


OD::LineStyle LogCurve::lineStyle() const
{
    return linestyle_;
}


void LogCurve::setDisplayRange( float left, float right )
{
    setDisplayRange( Interval<float>(left, right) );
}


void LogCurve::setDisplayRange( const Interval<float>& range )
{
    disprange_ = range;
    if ( axis_ )
	axis_->setRange( range );
}


void LogCurve::fillPar( IOPar& par ) const
{
    par.set( sKey::ID(), wellid_ );
    par.set( sKey::Log(), logname_);
    par.set( sKey::Range(), disprange_ );

    BufferString lsstr;
    lineStyle().toString( lsstr );
    par.set( sKey::LineStyle(), lsstr );
}


void LogCurve::usePar( const IOPar& par )
{
    par.get( sKey::ID(), wellid_ );
    par.get( sKey::Log(), logname_ );
    initLog();
    Interval<float> range;
    range.setUdf();
    par.get( sKey::Range(), range );
    setDisplayRange( range );

    BufferString lsstr;
    par.get( sKey::LineStyle(), lsstr );
    linestyle_.fromString( lsstr );
}
