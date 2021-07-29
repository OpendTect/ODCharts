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
				uiLogViewTable(uiParent*);
				~uiLogViewTable();

    int				currentView() const;
    void			setEmpty();
    bool			isEmpty() const;
    void			setViewLabel(int vwidx,const uiString&);
    void			addToViewLabel(int vwidx,const char*);
    int				size() const;
    bool			validIdx(int) const;

    void			addTrackCB(CallBacker*);
    void 			removeTrackCB(CallBacker*);
    void			updatePlotAreaCB(CallBacker*);
    void			updateZrangeCB(CallBacker*);
    void			clearSelection();

    Interval<float>		masterZRange() const;
    void			setMasterZRange(const Interval<float>,
						bool apply=true);

    uiLogChart*			getCurrentLogChart();
    uiLogChart*			getLogChart(int);
    uiLogView*			getCurrentLogView();
    uiLogView*			getLogView(int vwidx);

protected:
    uiTable*			logviews_;
    Interval<float>		masterzrange_;
    int				selected_ = -1;

    void			applyMasterZrange();

    void			addViewer(int);
    void			selectView(int);

    void			trackaddCB(CallBacker*);
    void			trackremoveCB(CallBacker*);
    void			colSelectCB(CallBacker*);
    void			selectCB(CallBacker*);
};
