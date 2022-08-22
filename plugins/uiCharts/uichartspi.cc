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
	"Isolation layer for QtCharts (GUI)",
	"OpendTect",
	"dGB Earth Sciences",
	"=od",
	""));
    return &retpi;
}


mDefODInitPlugin(uiCharts)
{
    return nullptr;
}
