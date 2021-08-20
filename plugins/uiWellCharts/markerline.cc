/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		August 2021
________________________________________________________________________

-*/


#include "markerline.h"

#include "draw.h"
#include "mnemonics.h"
#include "multiid.h"
#include "welldata.h"
#include "wellman.h"
#include "wellmarker.h"
#include "uichartaxes.h"
#include "uichartlabel.h"
#include "uilogchart.h"

MarkerLine::MarkerLine()
    : wellid_(MultiID::udf())
    , markername_(BufferString::empty())
{ }


MarkerLine::MarkerLine( const MultiID& wellid, const char* markernm )
    : wellid_(wellid)
    , markername_(markernm)
{
    initMarker();
}


MarkerLine::~MarkerLine()
{
    deleteAndZeroPtr( series_ );
    deleteAndZeroPtr( label_ );
}


bool MarkerLine::initMarker()
{
    Well::LoadReqs lreq( Well::Mrkrs );
    RefMan<Well::Data> wd = Well::MGR().get( wellid_, lreq );
    if ( !wd || !wd->markers().isPresent(markername_) )
	return false;

    wellname_ = wd->name();
    const Well::Marker* marker = wd->markers().getByName( markername_ );
    dah_ = marker->dah();
    linestyle_.color_ = marker->color();
    return true;
}


void MarkerLine::addTo( uiLogChart& logchart )
{
    addTo( logchart, linestyle_ );
}


void MarkerLine::addTo( uiLogChart& logchart, const OD::LineStyle& lstyle )
{
    if ( !series_ )
	addMarker( logchart );

    BufferString callouttxt( markername_ );
    callouttxt.add("(").add( wellname_ ).add( ") Depth: %2" );

    series_->setLineStyle( lstyle );
    logchart.addSeries( series_ );
    series_->attachAxis( logchart.getZAxis() );
    series_->setCalloutTxt( callouttxt );

    label_->setAnchor( Geom::PointF(0.f,dah_), mAlignment(Left,Bottom) );
    label_->updateCB(nullptr);
}


void MarkerLine::addTo( uiLogChart& logchart, const IOPar& iop )
{
    usePar( iop );
    addTo( logchart, linestyle_ );
}


void MarkerLine::removeFrom( uiLogChart& logchart )
{
    logchart.removeSeries( series_ );
    deleteAndZeroPtr( series_ );
    deleteAndZeroPtr( label_ );
}


void MarkerLine::setLineStyle(const OD::LineStyle& ls, bool usetransp)
{
    linestyle_ = ls;
    series_->setLineStyle( linestyle_, usetransp );
}


OD::LineStyle MarkerLine::lineStyle() const
{
    return linestyle_;
}


void MarkerLine::addMarker( uiLogChart& logchart )
{
    series_ = new uiLineSeries();
    label_ = new uiChartLabel( &logchart,
			       tr("%1 (%2)").arg(markername_).arg(wellname_),
			       series_ );
    const UnitOfMeasure* zdisp_uom = UnitOfMeasure::surveyDefDepthUnit();
    const UnitOfMeasure* zstor_uom = UnitOfMeasure::surveyDefDepthStorageUnit();
    dah_ = getConvertedValue(dah_, zstor_uom, zdisp_uom);

    series_->append( 0.f, dah_ );
    series_->append( 1.f, dah_ );
}


void MarkerLine::fillPar( IOPar& par ) const
{
    par.set( sKey::ID(), wellid_ );
    par.set( sKey::Name(), markername_);

    BufferString lsstr;
    lineStyle().toString( lsstr );
    par.set( sKey::LineStyle(), lsstr );
}


void MarkerLine::usePar( const IOPar& par )
{
    par.get( sKey::ID(), wellid_ );
    par.get( sKey::Name(), markername_ );
    initMarker();

    BufferString lsstr;
    par.get( sKey::LineStyle(), lsstr );
    linestyle_.fromString( lsstr );
}
