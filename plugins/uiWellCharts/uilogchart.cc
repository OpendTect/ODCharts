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

#include "chartutils.h"
#include "draw.h"
#include "logcurve.h"
#include "markerline.h"
#include "math2.h"
#include "multiid.h"
#include "separstr.h"
#include "survinfo.h"
#include "unitofmeasure.h"


uiLogChart::uiLogChart( uiWellCharts::ZType ztype, uiWellCharts::Scale scale )
    : logChange(this)
    , markerChange(this)
    , ztype_(ztype)
    , scale_(scale)
{
    makeZaxis();
}


uiLogChart::~uiLogChart()
{
    deepErase( logcurves_ );
    deepErase( markers_ );
}


bool uiLogChart::hasLogCurve( const MultiID& wellid, const char* lognm )
{
    bool found = false;
    for ( auto* logcurve : logcurves_ )
    {
	if ( logcurve->wellID()==wellid && logcurve->logName()==lognm )
	{
	    found = true;
	    break;
	}
    }

    return found;
}


LogCurve* uiLogChart::getLogCurve( const MultiID& wellid, const char* lognm )
{
    for ( auto* logcurve : logcurves_ )
    {
	if ( logcurve->wellID()==wellid && logcurve->logName()==lognm )
	    return logcurve;
    }

    return nullptr;
}


LogCurve* uiLogChart::getLogCurve( const char* lognm )
{
    for ( auto* logcurve : logcurves_ )
    {
	if ( logcurve->logName()==lognm )
	    return logcurve;
    }

    return nullptr;
}


void uiLogChart::addLogCurve( LogCurve* lc, bool show_wellnm, bool show_uom )
{
    if ( !lc )
	return;

    lc->addTo( *this, show_wellnm, show_uom );
    logcurves_ += lc;
    logChange.trigger();
}


void uiLogChart::addLogCurve( LogCurve* lc, const OD::LineStyle& lstyle,
			      bool show_wellnm, bool show_uom )
{
    if ( !lc )
	return;

    lc->addTo( *this, lstyle, show_wellnm, show_uom );
    logcurves_ += lc;
    logChange.trigger();
}


void uiLogChart::addLogCurves( ObjectSet<LogCurve>& lcs, bool show_wellnm,
			       bool show_uom)
{
    for ( auto* lc : lcs )
    {
	lc->addTo( *this, show_wellnm, show_uom );
	logcurves_ += lc;
    }

    logChange.trigger();
}


void uiLogChart::addLogCurve( const MultiID& wellid, const char* lognm )
{
    auto* logcurve = new LogCurve( wellid, lognm );
    logcurve->addTo( *this );
    logcurves_ += logcurve;
    logChange.trigger();
}


void uiLogChart::addLogCurve( const MultiID& wellid, const char* lognm,
			      const OD::LineStyle& lstyle )
{
    auto* logcurve = new LogCurve( wellid, lognm );
    logcurve->addTo( *this, lstyle );
    logcurves_ += logcurve;
    logChange.trigger();
}


void uiLogChart::addLogCurve( const MultiID& wellid, const char* lognm,
			      const OD::LineStyle& lstyle,
			      float min, float max, bool reverse,
			      bool show_wellnm, bool show_uom )
{
    auto* logcurve = new LogCurve( wellid, lognm );
    logcurve->addTo( *this, lstyle, min, max, reverse, show_wellnm, show_uom );
    logcurves_ += logcurve;
    logChange.trigger();
}


void uiLogChart::removeLogCurve( const char* lognm )
{
    for ( int idx=0; idx<logcurves_.size(); idx++ )
    {
	if ( logcurves_[idx]->logName()==lognm )
	{
	    LogCurve* tbremoved = logcurves_.removeSingle( idx );
	    tbremoved->removeFrom( *this );
	    delete tbremoved;
	    break;
	}
    }

    logChange.trigger();
}



void uiLogChart::removeLogCurve( const MultiID& wellid, const char* lognm )
{
    for ( int idx=0; idx<logcurves_.size(); idx++ )
    {
	if ( wellid==logcurves_[idx]->wellID() &&
		logcurves_[idx]->logName()==lognm )
	{
	    LogCurve* tbremoved = logcurves_.removeSingle( idx );
	    tbremoved->removeFrom( *this );
	    delete tbremoved;
	    break;
	}
    }

    logChange.trigger();
}


void uiLogChart::removeAllCurves()
{
    removeAllSeries();
    removeAllAxes( OD::Horizontal );
    deepErase( logcurves_ );
    logChange.trigger();
}


bool uiLogChart::hasMarker( const MultiID& wellid, const char* markernm )
{
    bool found = false;
    for ( auto* marker : markers_ )
    {
	if ( marker->wellID()==wellid && marker->markerName()==markernm )
	{
	    found = true;
	    break;
	}
    }

    return found;
}


void uiLogChart::addMarker( const MultiID& wellid, const char* markernm )
{
    auto* marker = new MarkerLine( wellid, markernm );
    marker->addTo( *this );
    markers_ += marker;
    markerChange.trigger();
}


void uiLogChart::addMarker( const MultiID& wellid, const char* markernm,
			      const OD::LineStyle& lstyle )
{
    addMarker( wellid, markernm );
    markers_.last()->setLineStyle( lstyle );
}


void uiLogChart::removeMarker( const MultiID& wellid, const char* markernm )
{
    for ( int idx=0; idx<markers_.size(); idx++ )
    {
	if ( wellid==markers_[idx]->wellID() &&
			markers_[idx]->markerName()==markernm )
	{
	    MarkerLine* tbremoved = markers_.removeSingle( idx );
	    tbremoved->removeFrom( *this );
	    delete tbremoved;
	    break;
	}
    }

    markerChange.trigger();
}


void uiLogChart::removeAllMarkers()
{
    for ( int idx=markers_.size()-1; idx>=0; idx++ )
    {
	MarkerLine* tbremoved = markers_.removeSingle( idx );
	tbremoved->removeFrom( *this );
	delete tbremoved;
    }
}


void uiLogChart::setZType( uiWellCharts::ZType ztype )
{
    if ( ztype_==ztype )
	return;

    ztype_ = ztype;
// TODO update logcurves to new ztype
    updateZAxisTitle();
}


uiValueAxis* uiLogChart::getZAxis() const
{
    return zaxis_;
}


Interval<float> uiLogChart::getActualZRange() const
{
    Interval<float> range;
    range.setUdf();
    for ( const auto* logcurve : logcurves_ )
	range.include( logcurve->zRange() );

    return range;
}


void uiLogChart::setZRange( float minz, float maxz )
{
    zaxis_->setRange( minz, maxz );
}


void uiLogChart::updateZAxisTitle()
{
    BufferString axtitle( uiWellCharts::toString(ztype_) );
    axtitle.addSpace();
    if ( ztype_==uiWellCharts::ZType::TWT )
	axtitle.add( UnitOfMeasure::surveyDefTimeUnitAnnot(true,true) );
    else
	axtitle.add( UnitOfMeasure::surveyDefDepthUnitAnnot(true,true) );

    zaxis_->setTitleText( axtitle );
}


void uiLogChart::setZRange( const Interval<float>& zrange )
{
    setZRange( zrange.start, zrange.stop );
}


void uiLogChart::makeZaxis()
{
    zaxis_ = new uiValueAxis;
    zaxis_->setTickType( uiValueAxis::TicksDynamic );
    zaxis_->setTickInterval( ztype_==uiWellCharts::ZType::TWT ? 0.1 :
					(SI().depthsInFeet() ? 500 : 200) );
    zaxis_->setMinorTickCount( 4 );
    zaxis_->setLabelFormat( "%d" );
    zaxis_->setRange( 0, 1000 );
    zaxis_->setReverse( true );
    updateZAxisTitle();
    addAxis( zaxis_, OD::Left );
}


uiChartAxis* uiLogChart::makeLogAxis( const BufferString& logtitle, float axmin,
				      float axmax, bool reverse )
{
    uiChartAxis* axis = nullptr;
    if ( scale_==uiWellCharts::Linear )
    {
	auto* vaxis = new uiValueAxis;
	vaxis->setTickType( uiValueAxis::TicksFixed );
	vaxis->setTickCount( 2 );
	vaxis->setMinorTickCount( 9 );
	vaxis->setLabelFormat( "%g" );
	vaxis->setRange( axmin, axmax );
	vaxis->setReverse( reverse );
	axis = vaxis;
    }
    else if ( scale_==uiWellCharts::Log10 )
    {
	auto* vaxis = new uiLogValueAxis;
	vaxis->setBase( 10 );
	vaxis->setMinorTickCount( 8 ); //TODO check
	vaxis->setLabelFormat( "%g" );
	if ( axmax<=0.f )
	    axmax = -axmax;
	if ( logcurves_.size() )
	{
	    Interval<float> maxis = logcurves_.first()->dispRange();
	    reverse = maxis.isRev();
	    const float mmax = maxis.isRev() ? maxis.start : maxis.stop;
	    const float mmin = maxis.isRev() ? maxis.stop : maxis.start;
	    const float mexp = Math::Log10(mmax) -
			       Math::Floor(Math::Log10(mmax/axmax));
	    axmax = Math::PowerOf( 10.f, mexp );
	    axmin = mmin/mmax*axmax;
	}
	vaxis->setRange( axmin, axmax );
	vaxis->setReverse( reverse );
	axis = vaxis;
    }

    if ( axis )
	axis->setTitleText( logtitle );

    return axis;
}


BufferStringSet uiLogChart::wellNames() const
{
    BufferStringSet res;
    for ( const auto* logcurve : logcurves_ )
	res.addIfNew( logcurve->wellName() );

    return res;
}


TypeSet<MultiID> uiLogChart::wellIDs() const
{
    TypeSet<MultiID> res;
    for ( const auto* logcurve : logcurves_ )
	res.addIfNew( logcurve->wellID() );

    return res;
}


BufferStringSet uiLogChart::getDispLogsForID( const MultiID& wellid ) const
{
    BufferStringSet res;
    for ( const auto* logcurve : logcurves_ )
    {
	if ( wellid==logcurve->wellID() )
	    res.addIfNew( logcurve->logName() );
    }
    return res;
}


BufferStringSet uiLogChart::getDispMarkersForID( const MultiID& wellid ) const
{
    BufferStringSet res;
    for ( const auto* marker : markers_ )
    {
	if ( wellid==marker->wellID() )
	    res.addIfNew( marker->markerName() );
    }
    return res;
}


void uiLogChart::setScale( uiWellCharts::Scale scaletyp )
{
    if ( scale_==scaletyp )
	return;

    scale_ =scaletyp;
    for ( auto* logcurve : logcurves_ )
    {
	logcurve->removeFrom( *this );
	const Interval<float> dr = logcurve->dispRange();
	float min = dr.isRev() ? dr.stop : dr.start;
	float max = dr.isRev() ? dr.start : dr.stop;
	bool reverse = dr.isRev();
	logcurve->addTo( *this, logcurve->lineStyle(), min, max, reverse );
    }
    logChange.trigger();
}


void uiLogChart::fillPar( IOPar& iop ) const
{
    iop.set( sKey::Type(), ztype_ );
    iop.set( sKey::Scale(), scale_ );
    FileMultiString zfms;
    zfms.add( zaxis_->getTickInterval() );
    zfms.add( zaxis_->gridVisible() );
    iop.set( sKey::ZMajorGrid(), zfms );

    BufferString lsstr;
    zaxis_->getGridStyle().toString( lsstr );
    iop.set( sKey::ZMajorGridStyle(), lsstr );
    zfms.setEmpty();
    zfms.add( zaxis_->getMinorTickCount() );
    zfms.add( zaxis_->minorGridVisible() );
    iop.set( sKey::ZMinorGrid(), zfms );
    zaxis_->getMinorGridStyle().toString( lsstr );
    iop.set( sKey::ZMinorGridStyle(), lsstr );

    int nitems = logcurves_.size();
    iop.set( sKey::NrItems(), nitems );
    if ( nitems<1 )
	return;

    uiChartAxis* laxis = logcurves_[0]->getAxis();
    FileMultiString lfms;
    lfms.add( laxis->getTickCount() );
    lfms.add( laxis->gridVisible() );
    iop.set( sKey::LogMajorGrid(), lfms );
    laxis->getGridStyle().toString( lsstr );
    iop.set( sKey::LogMajorGridStyle(), lsstr );

    lfms.setEmpty();
    lfms.add( laxis->getMinorTickCount() );
    lfms.add( laxis->minorGridVisible() );
    iop.set( sKey::LogMinorGrid(), lfms );
    laxis->getMinorGridStyle().toString( lsstr );
    iop.set( sKey::LogMinorGridStyle(), lsstr );

    for ( int idx=0; idx<nitems; idx++ )
    {
	IOPar tmp;
	logcurves_[idx]->fillPar( tmp );
	iop.mergeComp( tmp, IOPar::compKey(sKey::Log(), idx) );
    }


    int nmrkrs = markers_.size();
    iop.set( sKey::NrValues(), nmrkrs );
    if ( nmrkrs<1 )
	return;
    for ( int idx=0; idx<nmrkrs; idx++ )
    {
	IOPar tmp;
	markers_[idx]->fillPar( tmp );
	iop.mergeComp( tmp, IOPar::compKey(sKey::Marker(), idx) );
    }

}


void uiLogChart::usePar( const IOPar& iop )
{
    int ztype, scale;
    iop.get( sKey::Type(), ztype );
    ztype_ = uiWellCharts::ZTypeDef().getEnumForIndex( ztype );
    iop.get( sKey::Scale(), scale );
    scale_ = uiWellCharts::ScaleDef().getEnumForIndex( scale );

    FileMultiString zfms_major( iop.find(sKey::ZMajorGrid()) );
    FileMultiString zfms_minor( iop.find(sKey::ZMinorGrid()) );
    OD::LineStyle ls_major, ls_minor;
    BufferString lsstr;

    zaxis_->setTickInterval( zfms_major.getFValue(0) );
    zaxis_->setGridLineVisible( zfms_major.getYN(1) );
    zaxis_->setMinorTickCount( zfms_minor.getIValue(0) );
    zaxis_->setMinorGridLineVisible( zfms_minor.getYN(1) );

    iop.get( sKey::ZMajorGridStyle(), lsstr );
    ls_major.fromString( lsstr );
    iop.get( sKey::ZMinorGridStyle(), lsstr );
    ls_minor.fromString( lsstr );

    zaxis_->setGridStyle( ls_major );
    zaxis_->setMinorGridStyle( ls_minor );

    int nlog;
    iop.get( sKey::NrItems(), nlog );
    if ( nlog<1 )
	return;

    FileMultiString lfms_major( iop.find(sKey::LogMajorGrid()) );
    FileMultiString lfms_minor( iop.find(sKey::LogMinorGrid()) );
    iop.get( sKey::LogMajorGridStyle(), lsstr );
    ls_major.fromString( lsstr );
    iop.get( sKey::LogMinorGridStyle(), lsstr );
    ls_minor.fromString( lsstr );

    removeAllCurves();
    for ( int idx=0; idx<nlog; idx++ )
    {
	IOPar* tmp = iop.subselect( IOPar::compKey(sKey::Log(), idx) );
	auto* logcurve = new LogCurve;
	logcurve->addTo( *this, *tmp );
	uiChartAxis* laxis = logcurve->getAxis();
	laxis->setTickCount( lfms_major.getIValue(0) );
	laxis->setGridLineVisible( lfms_major.getYN(1) );
	laxis->setGridStyle( ls_major );
	laxis->setMinorTickCount( lfms_minor.getIValue(0) );
	laxis->setMinorGridLineVisible( lfms_minor.getYN(1) );
	laxis->setMinorGridStyle( ls_minor );
	logcurves_ += logcurve;
	logChange.trigger();
    }

    for ( auto* logcurve : logcurves_ )
	logcurve->addCurveFillTo( *this );

    int nmrkrs;
    iop.get( sKey::NrValues(), nmrkrs );
    if ( nmrkrs<1 )
	return;

    removeAllMarkers();
    for ( int idx=0; idx<nmrkrs; idx++ )
    {
	IOPar* tmp = iop.subselect( IOPar::compKey(sKey::Marker(), idx) );
	auto* marker = new MarkerLine;
	marker->addTo( *this, *tmp );
	markers_ += marker;
	markerChange.trigger();
    }

}
