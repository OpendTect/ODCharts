#pragma once

/*+
 * (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 * AUTHOR   : Nanne Hemstra
 * DATE     : November 2020
-*/

#include "uichartsmod.h"

#include "uiobj.h"

class ODChartView;
class uiChart;

mExpClass(uiCharts) uiChartView : public uiObject
{
public:
				uiChartView(uiParent*,const char* nm);
				~uiChartView();

protected:
    ODChartView&		mkbody(uiParent*,const char*);
    ODChartView*		odchartview_;

    uiChart*			uichart_;
};
