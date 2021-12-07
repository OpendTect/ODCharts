#pragma once
/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		June 2021
________________________________________________________________________

-*/

#include "uiwellchartsmod.h"
#include "uigroup.h"
#include "uilogchart.h"
#include "wellchartcommon.h"

class uiLogChart;
class uiLogView;
class uiTable;

mExpClass(uiWellCharts) uiLogViewTable : public uiGroup
{ mODTextTranslationClass(uiLogViewTable);
public:
				uiLogViewTable(uiParent*,bool showtools=false);
				~uiLogViewTable();

    int				currentView() const;
    void			setEmpty();
    bool			isEmpty() const;
    int				size() const;
    bool			validIdx(int) const;

    void			addTrackCB(CallBacker*);
    void			alignTopCB(CallBacker*);
    void 			removeTrackCB(CallBacker*);
    void			updatePrimaryChartCB(CallBacker*);
    void			updatePrimaryZrangeCB(CallBacker*);
    void			updateViewLabel(int);
    void			clearSelection();

    Interval<float>		primaryZRange() const;
    void			setPrimaryZRange(const Interval<float>,
						 bool apply=true);
    void			setZDomain(uiWellCharts::ZType);

    uiLogChart*			getCurrentLogChart();
    uiLogChart*			getLogChart(int);
    uiLogView*			getCurrentLogView();
    uiLogView*			getLogView(int vwidx);
    bool			isViewLocked(int vwidx);
    void			setViewLocked(int,bool yn=true);

    Notifier<uiLogViewTable>	chartSelectionChg;

protected:
    uiTable*			logviews_;
    uiLogChart*			primarychart_	= nullptr;
    bool			showtools_;
    Interval<float>		primaryzrange_	= Interval<float>::udf();
    int				selected_	= -1;

    void			addViewer(int);
    void			addTools(int);
    void			selectView(int);

    void			trackaddCB(CallBacker*);
    void			trackremoveCB(CallBacker*);
    void			colSelectCB(CallBacker*);
    void			selectCB(CallBacker*);
    void			syncViewsCB(CallBacker*);
};
