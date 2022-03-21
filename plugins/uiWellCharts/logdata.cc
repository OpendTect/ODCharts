/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		September 2021
________________________________________________________________________

-*/


#include "logdata.h"

#include "ioobj.h"
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


ConstRefMan<Well::Data> LogData::getWD() const
{
    Well::LoadReqs lreqs( Well::LogInfos );
    ConstRefMan<Well::Data> wd = Well::MGR().get( wellid_, lreqs );
    return wd;
}


bool LogData::initLog()
{
    ConstRefMan<Well::Data> wd = getWD();
    const Well::Log* log = wd ? wd->getLog( logname_ ) : nullptr;
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
    mnemlbl_ = log.mnemLabel();
    dahrange_ = log.dahRange();
    zrange_ = dahToZ( dahrange_, ztype_ );
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


void LogData::copyFrom( const LogData& oth )
{
    WellData::copyFrom( oth );
    uomlbl_ = oth.uomlbl_;
    logname_ = oth.logname_;
    mnemlbl_ = oth.mnemlbl_;
    dahrange_ = oth.dahrange_;
    zrange_ = oth.zrange_;
    valrange_ = oth.valrange_;
    disprange_ = oth.disprange_;
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
