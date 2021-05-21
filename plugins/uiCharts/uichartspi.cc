/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		November 2020
________________________________________________________________________

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
