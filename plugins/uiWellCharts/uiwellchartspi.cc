/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		November 2020
________________________________________________________________________

-*/

#include "uiwellchartsmod.h"

#include "odplugin.h"


mDefODPluginInfo(uiWellCharts)
{
    mDefineStaticLocalObject( PluginInfo, retpi,(
	"Charts for Well Data",
	"OpendTect",
	"dGB",
	"=od",
	""));
    return &retpi;
}


mDefODInitPlugin(uiWellCharts)
{
    return nullptr;
}
