/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		August 2020
________________________________________________________________________

-*/

#include "uilogview.h"

#include "uichart.h"
#include "logseries.h"
#include "welldata.h"
#include "welllog.h"
#include "welllogset.h"

uiLogView::uiLogView( uiParent* p, const char* nm )
    : uiChartView(p,nm)
{
    setZoomStyle( VerticalZoom );
}


uiLogView::~uiLogView()
{}


void uiLogView::setWellData( const Well::Data* wd )
{
    welldata_ = wd;

    BufferStringSet lognms;
    welldata_->logs().getNames( lognms );
    if ( lognms.isEmpty() )
	return;


    addLog( lognms.first()->buf() );
}


void uiLogView::addLog( const char* nm )
{
    const Well::Log* log = welldata_ ? welldata_->logs().getLog( nm ) : nullptr;
    if ( !log )
	return;

    auto* series = new LogSeries;
    series->setWellLog( *log );
    logseries_ += series;
    chart().addSeries( *series );
}


void uiLogView::removeLog( const char* nm )
{
    pErrMsg( "Not implemented yet" );
}
