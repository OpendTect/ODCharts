/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		November 2020
________________________________________________________________________

-*/

#include "uicharttoolsmod.h"

#include "uichartsfunctiondisplayserver.h"

#include "odplugin.h"


mDefODPluginInfo(uiChartTools)
{
    mDefineStaticLocalObject( PluginInfo, retpi,(
	"ChartTools",
	"OpendTect",
	"dGB",
	"=od",
	""));
    return &retpi;
}


mDefODInitPlugin(uiChartTools)
{
    GetFunctionDisplayServer( true, new uiChartsFunctionDisplayServer );
    return nullptr;
}
