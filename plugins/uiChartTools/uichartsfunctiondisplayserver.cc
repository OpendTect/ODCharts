/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:        Nanne Hemstra
 Date:          January 2022
________________________________________________________________________

-*/


#include "uichartsfunctiondisplayserver.h"
#include "uichartsfunctiondisplay.h"

uiChartsFunctionDisplayServer::uiChartsFunctionDisplayServer()
    : uiFunctionDisplayServer()
{}


uiChartsFunctionDisplayServer::~uiChartsFunctionDisplayServer()
{}


uiFuncDispBase*
	uiChartsFunctionDisplayServer::createFunctionDisplay( uiParent* p,
					const uiFuncDispBase::Setup& su )
{
    return new uiChartsFunctionDisplay( p, su );
}


uiFuncDrawerBase*
	uiChartsFunctionDisplayServer::createFunctionDrawer( uiParent* p,
					const uiFuncDrawerBase::Setup& su )
{
    return new uiChartsFunctionDrawer( p, su );
}
