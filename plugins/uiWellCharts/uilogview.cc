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
{
    deepUnRef( welldata_ );
    deepErase( logseries_ );
}


void uiLogView::addLog( const Well::Data& wd, const char* nm )
{
    const Well::Log* log = wd.logs().getLog( nm );
    if ( !log )
	return;

    wd.ref();
    welldata_ += &wd;
    auto* series = new LogSeries;
    series->setWellLog( *log );
    series->setName( BufferString(wd.name()," - ",log->name()) );
    logseries_ += series;
    chart().addSeries( *series );
}


void uiLogView::removeLog( const char* lognm )
{
    pErrMsg( "Not implemented yet" );
}
