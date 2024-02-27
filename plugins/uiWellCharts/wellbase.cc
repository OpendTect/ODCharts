/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "wellbase.h"

#include "ioobj.h"
#include "keystrs.h"
#include "multiid.h"
#include "ranges.h"
#include "unitofmeasure.h"
#include "welld2tmodel.h"
#include "welldata.h"
#include "wellman.h"
#include "welltrack.h"


using namespace uiWellCharts;

WellData::WellData()
{}


WellData::WellData( const MultiID& wellid )
{
    initWell( wellid );
}


WellData::~WellData()
{
}


bool WellData::initWell( const MultiID& wellid )
{
    if ( wellid.isUdf() )
	return false;

    wd_ = Well::MGR().get( wellid, Well::LoadReqs(Well::Inf) );
    if ( !wd_ )
	return false;

    ztype_ = uiWellCharts::MD;
    return true;
}


bool WellData::initWell( const char* wellnm )
{
    const IOObj* ioobj = Well::findIOObj( wellnm, nullptr );
    if ( !ioobj )
	return false;

    return initWell( ioobj->key() );
}


void WellData::copyFrom( const WellData& oth )
{
    ztype_ = oth.ztype_;
    wd_ = oth.wd_;
}


ConstRefMan<Well::Data> WellData::getWD() const
{
    return wd_;
}


void WellData::setZType( ZType ztype, bool )
{
    ztype_ = ztype;
}


float WellData::zToDah( float inz, ZType zt ) const
{
    const UnitOfMeasure* zduom = UnitOfMeasure::surveyDefDepthUnit();
    const UnitOfMeasure* zsuom = UnitOfMeasure::surveyDefDepthStorageUnit();
    if ( zt==MD )
	return getConvertedValue( inz, zduom, zsuom );

    Well::LoadReqs lreqs( Well::Trck );
    if ( zt==TWT )
	lreqs.add( Well::D2T );

    wd_ = Well::MGR().get( wellID(), lreqs );
    if ( !wd_ )
	return mUdf(float);

    const Well::Track& track = wd_->track();
    const Well::D2TModel* d2t = wd_->d2TModel();
    const bool istvd = zt==TVD || zt==TVDSS || zt==TVDSD;
    if ( istvd && track.isEmpty() )
	return mUdf(float);

    if ( zt==TWT && (!d2t || track.isEmpty()) )
	return mUdf(float);

    const UnitOfMeasure* ztuom = UnitOfMeasure::surveyDefTimeUnit();
    const UnitOfMeasure* ztsuom = nullptr;

    const float kb = track.getKbElev();
    const float srd = SI().seismicReferenceDatum();

    float zpos;
    if ( zt==TWT )
	zpos = getConvertedValue( inz, ztuom, ztsuom );
    else
	zpos = getConvertedValue( inz, zduom, zsuom );

    const float dah =
	zt==TVD ? track.getDahForTVD(zpos-kb)
		: zt==TVDSS ? track.getDahForTVD(zpos)
			    : zt==TVDSD ? track.getDahForTVD(zpos-srd)
					: d2t->getDah(zpos,track);

    return dah;
}


float WellData::dahToZ( float dah, ZType zt  ) const
{
    const UnitOfMeasure* zduom = UnitOfMeasure::surveyDefDepthUnit();
    const UnitOfMeasure* zsuom = UnitOfMeasure::surveyDefDepthStorageUnit();
    if ( zt==MD )
	return getConvertedValue( dah, zsuom, zduom );

    Well::LoadReqs lreqs( Well::Trck );
    if ( zt==TWT )
	lreqs.add( Well::D2T );

    wd_ = Well::MGR().get( wellID(), lreqs );
    if ( !wd_ )
	return mUdf(float);

    const Well::Track& track = wd_->track();
    const Well::D2TModel* d2t = wd_->d2TModel();
    const bool istvd = zt==TVD || zt==TVDSS || zt==TVDSD;
    if ( istvd && track.isEmpty() )
	return mUdf(float);

    if ( zt==TWT && (!d2t || track.isEmpty()) )
	return mUdf(float);

    const UnitOfMeasure* ztuom = UnitOfMeasure::surveyDefTimeUnit();
    const UnitOfMeasure* ztsuom = nullptr;

    const float kb = track.getKbElev();
    const float srd = SI().seismicReferenceDatum();
    const float zpos =
	zt==TVD ? track.getPos(dah).z+kb
		: zt==TVDSS ? track.getPos(dah).z
			    : zt==TVDSD ? track.getPos(dah).z+srd
					: d2t->getTime(dah,track);

    return zt==TWT ? getConvertedValue( zpos, ztsuom, ztuom )
		   : getConvertedValue( zpos, zsuom, zduom );
}


Interval<float> WellData::dahToZ( const Interval<float>& dahrg, ZType zt ) const
{
    const float zstart = dahToZ( dahrg.start, zt );
    const float zstop = dahToZ( dahrg.stop, zt );
    return Interval<float>( zstart, zstop );
}


void WellData::fillPar( IOPar& par ) const
{
    if ( !wd_ )
	return;

    par.set( sKey::ID(), wellID() );
}


void WellData::usePar( const IOPar& par )
{
    MultiID wellid;
    par.get( sKey::ID(), wellid );
    initWell( wellid );
}
