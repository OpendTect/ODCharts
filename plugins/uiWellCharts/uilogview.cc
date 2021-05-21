/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		November 2020
________________________________________________________________________

-*/

#include "uilogview.h"

#include "uichart.h"
#include "uilogchart.h"
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
}


void uiLogView::setLogChart( uiLogChart* newchart )
{
    setChart( newchart );
}


uiLogChart* uiLogView::logChart()
{
    uiChart* ch = chart();
    return dynamic_cast<uiLogChart*>(ch);
}
