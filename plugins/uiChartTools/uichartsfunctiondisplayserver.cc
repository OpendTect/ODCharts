/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
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
