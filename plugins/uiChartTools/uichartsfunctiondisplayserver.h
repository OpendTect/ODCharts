#pragma once
/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:        Nanne Hemstra
 Date:          January 2022
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
};
