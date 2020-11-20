/*+
 * (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 * AUTHOR   : Raman Singh
 * DATE     : June 2008
-*/

#include "uiwellchartsmod.h"

#include "odplugin.h"


mDefODPluginInfo(uiWellCharts)
{
    mDefineStaticLocalObject( PluginInfo, retpi,(
	"GMT Link (GUI)",
	"OpendTect",
	"dGB (Raman)",
	"3.2",
	"A link to the GMT mapping tool."
	    "\nThis is the User interface of the link."
	    "\nSee http://opendtect.org/links/gmt.html for info on GMT" ));
    return &retpi;
}


mDefODInitPlugin(uiWellCharts)
{
    return nullptr;
}
