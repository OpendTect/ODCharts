/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "markerline.h"

#include "uichartaxes.h"
#include "uichartlabel.h"
#include "uilogchart.h"

#include "draw.h"
#include "keystrs.h"
#include "multiid.h"
#include "wellbase.h"
#include "welldata.h"
#include "wellman.h"
#include "wellmarker.h"


MarkerLine::MarkerLine()
    : WellData()
    , markername_(BufferString::empty())
{ }


MarkerLine::MarkerLine( const MultiID& wellid, const char* markernm )
    : WellData(wellid)
    , markername_(markernm)
{
    valid_ = initMarker();
}


MarkerLine::~MarkerLine()
{
    deleteAndNullPtr( series_ );
    deleteAndNullPtr( label_ );
}


bool MarkerLine::initMarker()
{
    wd_ = Well::MGR().get( wellID(), Well::LoadReqs(Well::Mrkrs) );
    if ( !wd_ || !wd_->markers().isPresent(markername_) )
	return false;

    const Well::Marker* marker = wd_->markers().getByName( markername_ );
    dah_ = marker->dah();
    zpos_ = dahToZ( dah_, ztype_ );
    linestyle_.color_ = marker->color();
    return true;
}


void MarkerLine::addTo( uiLogChart& logchart, bool show_wellnm )
{
    addTo( logchart, linestyle_, show_wellnm );
}


void MarkerLine::addTo( uiLogChart& logchart, const OD::LineStyle& lstyle,
			bool show_wellnm )
{
    if ( !valid_ )
	return;

    if ( !series_ )
	addMarker( logchart, show_wellnm );

    BufferString callouttxt( markername_ );
    if ( show_wellnm )
	callouttxt.add("(").add(wellName()).add(")");

    callouttxt.add(" Depth: %2" );

    series_->setLineStyle( lstyle );
    logchart.addSeries( series_ );
    series_->attachAxis( logchart.getZAxis() );
    series_->setCalloutTxt( callouttxt );
    setZType( logchart.zType(), false );
    logchart.needsRedraw.trigger();
}


void MarkerLine::addTo( uiLogChart& logchart, const IOPar& iop )
{
    usePar( iop );
    addTo( logchart, linestyle_ );
}


void MarkerLine::removeFrom( uiLogChart& logchart )
{
    if ( !valid_ )
	return;

    logchart.removeSeries( series_ );
    deleteAndNullPtr( series_ );
    deleteAndNullPtr( label_ );
}


void MarkerLine::setZType( uiWellCharts::ZType ztype, bool force )
{
    if ( !valid_ || (!force && ztype==ztype_) )
	return;

    WellData::setZType( ztype, true );
    zpos_ = dahToZ( dah_, ztype );
    series_->setAll_Y( zpos_ );
    label_->setAnchor( Geom::PointF(0.f,zpos_), mAlignment(Left,Bottom) );
    label_->updateCB(nullptr);
}


void MarkerLine::setLineStyle(const OD::LineStyle& ls, bool usetransp)
{
    linestyle_ = ls;
    if ( series_ )
	series_->setLineStyle( linestyle_, usetransp );
}


OD::LineStyle MarkerLine::lineStyle() const
{
    return linestyle_;
}


void MarkerLine::addMarker( uiLogChart& logchart, bool show_wellnm )
{
    if ( !valid_ )
	return;

    series_ = new uiLineSeries();
    uiString lblstr;
    if ( show_wellnm )
	lblstr = tr("%1 (%2)").arg(markername_).arg(wellName());
    else
	lblstr = tr("%1").arg(markername_);

    label_ = new uiChartLabel( &logchart, lblstr, series_ );
    series_->append( 0.f, zpos_ );
    series_->append( 1.f, zpos_ );
    setZType( ztype_, true );
}


void MarkerLine::fillPar( IOPar& par ) const
{
    WellData::fillPar( par );
    par.set( sKey::Name(), markername_);

    BufferString lsstr;
    lineStyle().toString( lsstr );
    par.set( sKey::LineStyle(), lsstr );
}


void MarkerLine::usePar( const IOPar& par, bool styleonly )
{
    if ( !styleonly )
    {
	WellData::usePar( par );
	par.get( sKey::Name(), markername_ );
	valid_ = initMarker();
    }

    BufferString lsstr;
    par.get( sKey::LineStyle(), lsstr );
    OD::LineStyle ls;
    ls.fromString( lsstr );
    setLineStyle( ls );
}
