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
#include "multiid.h"
#include "separstr.h"

mDefineEnumUtils(uiLogChart, Scale, "Log chart scale type")
    { "Linear", "Log10", 0 };

mDefineEnumUtils(uiLogChart, ZType, "Log chart Z type")
    { "MD", "TVD", "TVDSS", "TWT" };

uiLogChart::uiLogChart( ZType ztype, Scale scale )
    : logChange(this)
    , ztype_(ztype)
    , scale_(scale)
{
    makeZaxis();
}


uiLogChart::~uiLogChart()
{
    deepErase( logcurves_ );
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


void uiLogChart::addLogCurve( const MultiID& wellid, const char* lognm )
{
    LogCurve* logcurve = new LogCurve( wellid, lognm );
    logcurve->addTo( *this );
    logcurves_ += logcurve;
    logChange.trigger();
}


void uiLogChart::addLogCurve( const MultiID& wellid, const char* lognm,
			      const OD::LineStyle& lstyle )
{
    LogCurve* logcurve = new LogCurve( wellid, lognm );
    logcurve->addTo( *this, lstyle );
    logcurves_ += logcurve;
    logChange.trigger();
}


void uiLogChart::addLogCurve( const MultiID& wellid, const char* lognm,
			      const OD::LineStyle& lstyle,
			      float min, float max, bool reverse )
{
    LogCurve* logcurve = new LogCurve( wellid, lognm );
    logcurve->addTo( *this, lstyle, min, max, reverse );
    logcurves_ += logcurve;
    logChange.trigger();
}


void uiLogChart::removeAllCurves()
{
    removeAllSeries();
    removeAllAxes( OD::Horizontal );
    deepErase( logcurves_ );
    logChange.trigger();
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
	range.include(logcurve->zRange() );

    return range;
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


BufferStringSet uiLogChart::wellNames() const
{
    BufferStringSet res;
    for ( auto* logcurve : logcurves_ )
	res.addIfNew( logcurve->wellName() );

    return res;
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
}


void uiLogChart::usePar( const IOPar& iop )
{
    int ztype, scale;
    iop.get( sKey::Type(), ztype );
    ztype_ = ZTypeDef().getEnumForIndex( ztype );
    iop.get( sKey::Scale(), scale );
    scale_ = ScaleDef().getEnumForIndex( scale );

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
	LogCurve* logcurve = new LogCurve;
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
}
