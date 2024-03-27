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
#include "welldata.h"
#include "welllog.h"
#include "wellman.h"

LogData::LogData()
    : WellData()
    , logname_(BufferString::empty())
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
    if ( !wd_ )
	return false;

    const Well::Log* log = wd_->getLog( logname_ );
    if ( !log )
	return false;

    return initLog( *log );
}


bool LogData::initLog( const char* wellnm, const Well::Log& log )
{
    if ( !initWell(wellnm) )
	return false;

    return initLog( log );
}

bool LogData::initLog( const Well::Log& log )
{
    logname_ = log.name();
    uomlbl_ = log.unitMeasLabel();
    mnemlbl_ = log.mnemonicLabel();
    dahrange_ = log.dahRange();
    zrange_ = dahToZ( dahrange_, ztype_ );
    valrange_ = log.valueRange();
    disprange_.setUdf();
    logmnem_ = log.mnemonic();
    loguom_ = log.unitOfMeasure();
    if ( logmnem_ )
	dispuom_ = logmnem_->getDisplayInfo( dispscale_, disprange_, displbl_,
					     linestyle_ );

    if ( !loguom_ )
    {
	dispuom_ = nullptr;
	disprange_.start = valrange_.start;
	disprange_.stop = valrange_.stop;
    }
    return true;
}


void LogData::copyFrom( const LogData& oth )
{
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
}


Interval<float> LogData::dispRange()
{
    if ( disprange_.isUdf() )
    {
	StepInterval<float> ni( valrange_ );
	if ( loguom_ && dispuom_ )
	{
	    ni.start = getConvertedValue( ni.start, loguom_, dispuom_ );
	    ni.stop = getConvertedValue( ni.stop, loguom_, dispuom_ );
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
