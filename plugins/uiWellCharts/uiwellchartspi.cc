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
	"Charts for Well Data (GUI)",
	"OpendTect",
	"dGB Earth Sciences",
	"=od",
	"" ))
    return &retpi;
}


mDefODInitPlugin(uiWellCharts)
{
    return nullptr;
}
