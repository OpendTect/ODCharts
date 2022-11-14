/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uichartsmod.h"

#include "odplugin.h"


mDefODPluginInfo(uiCharts)
{
    mDefineStaticLocalObject( PluginInfo, retpi, (
	"Charts (GUI)",
	"OpendTect Pro",
	"dGB Earth Sciences",
	"=od",
	"Isolation layer for QtCharts"));
    return &retpi;
}


mDefODInitPlugin(uiCharts)
{
    return nullptr;
}
