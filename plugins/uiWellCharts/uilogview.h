#pragma once
/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uiwellchartsmod.h"
#include "uichartview.h"

class uiLogChart;
class uiLogViewPropDlg;

mExpClass(uiWellCharts) uiLogView : public uiChartView
{
public:
			uiLogView(uiParent*,const char* nm);
			~uiLogView();

    void		setLogChart(uiLogChart*);
    uiLogChart*		logChart();

    void		showSettingsCB(CallBacker*);
    void		zoomResetCB(CallBacker*);

protected:
    uiLogViewPropDlg*	propdlg_ = nullptr;

};
