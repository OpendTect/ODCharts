/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		November 2020
________________________________________________________________________

-*/

#include "uicharttoolsmod.h"

#include "uichartsfunctiondisplayserver.h"

#include "ioman.h"
#include "odplugin.h"


mDefODPluginInfo(uiChartTools)
{
    mDefineStaticLocalObject( PluginInfo, retpi, (
	"Charts Tools (GUI)",
	"OpendTect",
	"dGB Earth Sciences",
	"=od",
	"" ))
    return &retpi;
}


class uiChartToolsPIMgr : public CallBacker
{
public:
uiChartToolsPIMgr()
{
    IOM().afterSurveyChange.notify(
	mCB(this,uiChartToolsPIMgr,afterSurveyChangeCB), true );
    activateDisplayServer();
}


void afterSurveyChangeCB( CallBacker* )
{
    activateDisplayServer();
}


void activateDisplayServer()
{
    GetFunctionDisplayServer( true, new uiChartsFunctionDisplayServer );
}


protected:
};

mDefODInitPlugin(uiChartTools)
{
    mDefineStaticLocalObject( PtrMan<uiChartToolsPIMgr>, mgr,
			      = new uiChartToolsPIMgr() )
    return nullptr;
}
