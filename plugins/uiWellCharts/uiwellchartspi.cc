/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uiwellchartsmod.h"

#include "odplugin.h"


mDefODPluginInfo(uiWellCharts)
{
    mDefineStaticLocalObject( PluginInfo, retpi,(
	"Charts - Well Data (GUI)",
	"OpendTect Pro",
	"dGB Earth Sciences",
	"=od",
	"This plugin provides charts for well data, like log displays" ))
    return &retpi;
}


mDefODInitPlugin(uiWellCharts)
{
    return nullptr;
}
