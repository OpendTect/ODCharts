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

class uiLogChart;
class uiLogView;
class uiTable;

mExpClass(uiWellCharts) uiLogViewTable : public uiGroup
{ mODTextTranslationClass(uiLogViewTable);
public:
				uiLogViewTable(uiParent*,bool showTools=false);
				~uiLogViewTable();

    int				currentView() const;
    void			setEmpty();
    bool			isEmpty() const;
    int				size() const;
    bool			validIdx(int) const;

    void			addTrackCB(CallBacker*);
    void 			removeTrackCB(CallBacker*);
    void			updatePlotAreaCB(CallBacker*);
    void			updateMasterZrangeCB(CallBacker*);
    void			updateViewLabel(int);
    void			clearSelection();

    Interval<float>		masterZRange() const;
    void			setMasterZRange(const Interval<float>,
						bool apply=true);

    uiLogChart*			getCurrentLogChart();
    uiLogChart*			getLogChart(int);
    uiLogView*			getCurrentLogView();
    uiLogView*			getLogView(int vwidx);
    bool			isViewLocked(int vwidx);

    Notifier<uiLogViewTable>	chartSelectionChg;

protected:
    uiTable*			logviews_;
    bool			showtools_;
    Interval<float>		masterzrange_;
    int				selected_ = -1;

    void			addViewer(int);
    void			addTools(int);
    void			selectView(int);

    void			trackaddCB(CallBacker*);
    void			trackremoveCB(CallBacker*);
    void			colSelectCB(CallBacker*);
    void			selectCB(CallBacker*);
    void			syncViewsCB(CallBacker*);
};
