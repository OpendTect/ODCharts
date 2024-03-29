/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uicharttoolsmod.h"

#include "uichartsfunctiondisplayserver.h"
#include "uiwellchartsdisplayserver.h"

#include "ioman.h"
#include "odplugin.h"


mDefODPluginInfo(uiChartTools)
{
    mDefineStaticLocalObject( PluginInfo, retpi, (
	"Charts - Tools (GUI)",
	"OpendTect Pro",
	"dGB Earth Sciences",
	"=od",
	"Common tools for Charts plugin" ))
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
    GetWellDisplayServer( true, new uiWellChartsDisplayServer );
}


protected:
};

mDefODInitPlugin(uiChartTools)
{
    mDefineStaticLocalObject( PtrMan<uiChartToolsPIMgr>, mgr,
			      = new uiChartToolsPIMgr() )
    return nullptr;
}
