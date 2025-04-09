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
class uiLogViewTable;

mExpClass(uiWellCharts) uiLogView : public uiChartView
{ mODTextTranslationClass(uiLogView)
public:
			uiLogView(uiParent*,const char* nm,
				  bool applyall=true);
			~uiLogView();

    void		setLogChart(uiLogChart*);
    uiLogChart*		logChart();
    void		setLogViewTable(uiLogViewTable*);

    void		showSettingsCB(CallBacker*);
    void		zoomResetCB(CallBacker*);

protected:
    uiLogViewPropDlg*	propdlg_ = nullptr;
    uiLogViewTable* 	logviewtbl_ = nullptr;
    bool		applyall_;

    void		applySettingsCB(CallBacker*);
};
