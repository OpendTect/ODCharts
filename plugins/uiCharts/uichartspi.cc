/*+
 * (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 * AUTHOR   : Raman Singh
 * DATE     : June 2008
-*/

static const char* rcsID mUsedVar = "$Id$";

#include "uichartsmod.h"

#include "odplugin.h"


mDefODPluginInfo(uiCharts)
{
    mDefineStaticLocalObject( PluginInfo, retpi,(
	"Isolation layer for QtCharts",
	"OpendTect",
	"dGB (Nanne)",
	"=od",
	""));
    return &retpi;
}


mDefODInitPlugin(uiCharts)
{
    return nullptr;
}
