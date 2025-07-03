#pragma once
/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uicharttoolsmod.h"

#include "uifunctiondisplayserver.h"

mExpClass(uiChartTools) uiChartsFunctionDisplayServer :
				public uiFunctionDisplayServer
{
public:
			uiChartsFunctionDisplayServer();
			~uiChartsFunctionDisplayServer();

    uiFuncDispBase*	createFunctionDisplay(uiParent*,
				    const uiFuncDispBase::Setup&) override;
    uiFuncDrawerBase*	createFunctionDrawer(uiParent*,
				    const uiFuncDrawerBase::Setup&) override;
    uiMultiFuncDispBase* createMultiFunctionDisplay(uiParent*,
				    const uiMultiFuncDispBase::Setup&) override;
};
