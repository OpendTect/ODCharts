/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		September 2021
________________________________________________________________________

-*/


#include "logdata.h"

#include "mnemonics.h"
#include "multiid.h"
#include "ranges.h"
#include "welldata.h"
#include "welllog.h"
#include "wellman.h"

LogData::LogData()
    : wellid_(MultiID::udf())
    , logname_(BufferString::empty())
{}


LogData::LogData( const MultiID& wellid, const char* lognm )
    : wellid_(wellid)
    , logname_(lognm)
{
    initLog();
}


LogData::~LogData()
{
}


bool LogData::initLog()
{
    Well::LoadReqs lreq( Well::LogInfos );
    RefMan<Well::Data> wd = Well::MGR().get( wellid_, lreq );
    if ( !wd || !wd->getLog(logname_) )
	return false;

    const Well::Log* log = wellLog();
    if ( !log )
	return false;

    return initLog( wd->name(), *log );
}


bool LogData::initLog( const char* wellnm, const Well::Log& log )
{
    wellname_ = wellnm;
    logname_ = log.name();
    uomlbl_ = log.unitMeasLabel();
    mnemlbl_ = log.mnemLabel();
    dahrange_ = log.dahRange();
    valrange_ = log.valueRange();
    disprange_.setUdf();

    const Mnemonic* mnem = log.mnemonic();
    if ( mnem )
    {
	const Mnemonic::DispDefs& disp = mnem->disp_;
	const UnitOfMeasure* mnem_uom = mnem->unit();
	const UnitOfMeasure* log_uom = log.unitOfMeasure();

	disprange_.start = getConvertedValue( disp.typicalrange_.start,
					      mnem_uom, log_uom );
	disprange_.stop = getConvertedValue( disp.typicalrange_.stop,
					     mnem_uom, log_uom );
    }
    else
    {
	disprange_.start = valrange_.start;
	disprange_.stop = valrange_.stop;
    }
    return true;
}


const Well::Log* LogData::wellLog() const
{
    Well::LoadReqs lreq( Well::LogInfos );
    RefMan<Well::Data> wd = Well::MGR().get( wellid_, lreq );
    return wd ? wd->getLog( logname_ ) : nullptr;
}


Interval<float> LogData::dispRange()
{
    if ( disprange_.isUdf() )
    {
	const StepInterval<float> ni( valrange_ );
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


void LogData::setLogName( const char* nm )
{
    logname_ = nm;
}


void LogData::fillPar( IOPar& par ) const
{
    par.set( sKey::ID(), wellid_ );
    par.set( sKey::Log(), logname_);
    par.set( sKey::Range(), disprange_ );
}


void LogData::usePar( const IOPar& par )
{
    par.get( sKey::ID(), wellid_ );
    par.get( sKey::Log(), logname_ );
    initLog();
    Interval<float> range;
    range.setUdf();
    par.get( sKey::Range(), range );
    setDisplayRange( range );
}
