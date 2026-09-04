/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "logdata.h"

#include "keystrs.h"
#include "mnemonics.h"
#include "multiid.h"
#include "ranges.h"
#include "unitofmeasure.h"
#include "welldata.h"
#include "welllogset.h"
#include "wellman.h"

LogData::LogData()
    : WellData()
{}


LogData::LogData( const MultiID& wellid, const char* lognm )
    : WellData(wellid)
    , logname_(lognm)
{
    initLog();
}


LogData::~LogData()
{
}


bool LogData::initLog()
{
    return initLog( logname_.buf() );
}


bool LogData::initLog( const char* wellnm, const char* logname )
{
    if ( !initWell(wellnm) )
	return false;

    return initLog( logname );
}

bool LogData::initLog( const char* logname )
{
    if ( !wd_ )
	return false;

    const Well::LogSet& logs = wd_->logs();
    if ( !logs.isPresent(logname) )
	return false;

    logname_ = logname;
    uomlbl_ = logs.getUnitOfMeasureLblOfLog( logname );
    mnemlbl_ = logs.getMnemonicLblOfLog( logname );
    dahrange_ = logs.getDahRangeForLog( logname );
    zrange_ = dahToZ( dahrange_, ztype_ );
    valrange_ = logs.getValueRangeForLog( logname );
    disprange_.setUdf();
    logmnem_ = logs.getMnemonicOfLog( logname );
    loguom_ = logs.getUnitOfMeasureOfLog( logname );
    if ( logmnem_ )
	dispuom_ = logmnem_->getDisplayInfo( dispscale_, disprange_, displbl_,
					     linestyle_ );

    if ( !loguom_ )
    {
	dispuom_ = nullptr;
	disprange_.start_ = valrange_.start_;
        disprange_.stop_ = valrange_.stop_;
    }

    return true;
}


LogData& LogData::copyFrom( const LogData& oth )
{
    if ( &oth == this )
	return *this;

    WellData::copyFrom( oth );
    uomlbl_ = oth.uomlbl_;
    logname_ = oth.logname_;
    mnemlbl_ = oth.mnemlbl_;
    dahrange_ = oth.dahrange_;
    zrange_ = oth.zrange_;
    valrange_ = oth.valrange_;
    logmnem_ = oth.logmnem_;
    loguom_ = oth.loguom_;
    dispscale_ =oth.dispscale_;
    dispuom_ = oth.dispuom_;
    displbl_ = oth.displbl_;
    disprange_ = oth.disprange_;
    linestyle_ = oth.linestyle_;
    return *this;
}


Interval<float> LogData::dispRange()
{
    if ( disprange_.isUdf() )
    {
	StepInterval<float> ni( valrange_ );
	if ( loguom_ && dispuom_ )
	{
	    ni.start_ = getConvertedValue( ni.start_, loguom_, dispuom_ );
            ni.stop_ = getConvertedValue( ni.stop_, loguom_, dispuom_ );
	}
	disprange_ = ni.niceInterval( 10, false );
    }
    return disprange_;
}


void LogData::setDisplayRange( float left, float right )
{
    setDisplayRange( Interval<float>(left,right) );
}


void LogData::setDisplayRange( const Interval<float>& range )
{
    disprange_ = range;
}


void LogData::setZType( uiWellCharts::ZType ztype, bool force )
{
    if ( !force && ztype_==ztype )
	return;

    WellData::setZType( ztype, force );
    zrange_ = dahToZ( dahrange_, ztype );
}


void LogData::setLogName( const char* nm )
{
    logname_ = nm;
}


void LogData::fillPar( IOPar& par ) const
{
    WellData::fillPar( par );
    par.set( sKey::Log(), logname_);
    par.set( sKey::Range(), disprange_ );
}


void LogData::usePar( const IOPar& par, bool styleonly )
{
    if ( !styleonly )
    {
	WellData::usePar( par );
	par.get( sKey::Log(), logname_ );
	initLog();
    }
    Interval<float> range;
    range.setUdf();
    par.get( sKey::Range(), range );
    setDisplayRange( range );
}
