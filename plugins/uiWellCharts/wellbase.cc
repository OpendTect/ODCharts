/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		December 2021
________________________________________________________________________

-*/


#include "logdata.h"

#include "ioobj.h"
#include "mnemonics.h"
#include "multiid.h"
#include "ranges.h"
#include "wellbase.h"
#include "welld2tmodel.h"
#include "welldata.h"
#include "welllog.h"
#include "wellman.h"
#include "welltrack.h"


using namespace uiWellCharts;

WellData::WellData()
    : wellid_(MultiID::udf())
{}


WellData::WellData( const MultiID& wellid )
    : wellid_(wellid)
{
    initWell();
}


WellData::~WellData()
{
}


bool WellData::initWell()
{
    Well::LoadReqs lreq( Well::Inf );
    RefMan<Well::Data> wd = Well::MGR().get( wellid_, lreq );
    if ( !wd )
	return false;

    wellname_ = wd->name();
    ztype_ = uiWellCharts::MD;
    return true;
}


bool WellData::initWell( const char* wellnm )
{
    const IOObj* ioobj = Well::findIOObj( wellnm, nullptr );
    if ( !ioobj )
	return false;

    wellid_ = ioobj->key();
    return initWell();
}


void WellData::copyFrom( const WellData& oth )
{
    ztype_ = oth.ztype_;
    wellid_ = oth.wellid_;
    wellname_ = oth.wellname_;
}


const Well::Track* WellData::wellTrack() const
{
    if ( wellid_.isUdf() )
	return nullptr;

    Well::LoadReqs lreq( Well::Trck );
    RefMan<Well::Data> wd = Well::MGR().get( wellid_, lreq );
    return wd ? &wd->track() : nullptr;
}


const Well::D2TModel* WellData::wellD2TModel() const
{
    if ( wellid_.isUdf() )
	return nullptr;

    Well::LoadReqs lreq( Well::D2T );
    RefMan<Well::Data> wd = Well::MGR().get( wellid_, lreq );
    return wd ? wd->d2TModel() : nullptr;
}


void WellData::setZType( ZType ztype, bool )
{
    ztype_ = ztype;
}


float WellData::zToDah( float inz, ZType zt  )
{
    const UnitOfMeasure* zduom = UnitOfMeasure::surveyDefDepthUnit();
    const UnitOfMeasure* zsuom = UnitOfMeasure::surveyDefDepthStorageUnit();
    if ( zt==MD )
	return getConvertedValue( inz, zduom, zsuom );

    const Well::Track* track = wellTrack();
    const Well::D2TModel* d2t = wellD2TModel();
    const bool istvd = zt==TVD || zt==TVDSS || zt==TVDSD;
    if ( istvd && !track )
	return mUdf(float);

    if ( zt==TWT && (!d2t || !track) )
	return mUdf(float);

    const UnitOfMeasure* ztuom = UnitOfMeasure::surveyDefTimeUnit();
    const UnitOfMeasure* ztsuom = nullptr;

    const float kb = track->getKbElev();
    const float srd = SI().seismicReferenceDatum();

    float zpos;
    if ( zt==TWT )
	zpos = getConvertedValue( inz, ztuom, ztsuom );
    else
	zpos = getConvertedValue( inz, zduom, zsuom );

    const float dah = zt==TVD ? track->getDahForTVD(zpos-kb) :
			zt==TVDSS ? track->getDahForTVD(zpos) :
			zt==TVDSD ? track->getDahForTVD(zpos-srd) :
			d2t->getDah(zpos, *track);

    return dah;
}


float WellData::dahToZ( float dah, ZType zt  )
{
    const UnitOfMeasure* zduom = UnitOfMeasure::surveyDefDepthUnit();
    const UnitOfMeasure* zsuom = UnitOfMeasure::surveyDefDepthStorageUnit();
    if ( zt==MD )
	return getConvertedValue( dah, zsuom, zduom );

    const Well::Track* track = wellTrack();
    const Well::D2TModel* d2t = wellD2TModel();
    const bool istvd = zt==TVD || zt==TVDSS || zt==TVDSD;
    if ( istvd && !track )
	return mUdf(float);

    if ( zt==TWT && (!d2t || !track) )
	return mUdf(float);

    const UnitOfMeasure* ztuom = UnitOfMeasure::surveyDefTimeUnit();
    const UnitOfMeasure* ztsuom = nullptr;

    const float kb = track->getKbElev();
    const float srd = SI().seismicReferenceDatum();
    const float zpos = 	zt==TVD ? track->getPos(dah).z+kb :
			zt==TVDSS ? track->getPos(dah).z :
			zt==TVDSD ? track->getPos(dah).z+srd :
			d2t->getTime( dah, *track );

    return zt==TWT ? getConvertedValue( zpos, ztsuom, ztuom )
		   : getConvertedValue( zpos, zsuom, zduom );
}


Interval<float> WellData::dahToZ( const Interval<float>& dahrg, ZType zt )
{
    const float zstart = dahToZ( dahrg.start, zt );
    const float zstop = dahToZ( dahrg.stop, zt );
    return Interval<float>( zstart, zstop );
}


void WellData::fillPar( IOPar& par ) const
{
    par.set( sKey::ID(), wellid_ );
}


void WellData::usePar( const IOPar& par )
{
    par.get( sKey::ID(), wellid_ );
    initWell();
}