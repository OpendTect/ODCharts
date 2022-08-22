#pragma once
/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
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
				uiLogViewTable(uiParent*,int nrcol=0,
					       bool showtools=false);
				~uiLogViewTable();

    int				currentView() const;
    void			setCurrentView(int);
    void			setEmpty();
    bool			isEmpty() const;
    int				size() const;
    bool			validIdx(int) const;
    void			setNumViews(int);

    void			addWellData(const BufferStringSet&,
					    const ObjectSet<const Well::Log>&,
					    const OD::LineStyle&,
					    const char* lognmsuffix=nullptr);
    void			addWellData(const DBKeySet&,
					const ManagedObjectSet<TypeSet<int>>&);
    void			addWellData(const DBKeySet&,
					const ManagedObjectSet<TypeSet<int>>&,
					    const BufferStringSet& mrknms);
    void			addWellData(const DBKeySet&,
					    const BufferStringSet& lognms,
					    const BufferStringSet& mrknms);
    void			addWellData(const DBKeySet&,
					    const BufferStringSet&,
					    const OD::LineStyle&);

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
    bool			isViewLocked(int vwidx) const;
    void			setViewLocked(int,bool yn=true);
    bool			isViewVisible(int vwidx) const;
    bool			allLocked() const	{ return alllocked_; }
    void			setAllLocked(bool yn);

    Notifier<uiLogViewTable>	chartSelectionChg;

protected:
    uiTable*			logviews_;
    uiLogChart*			primarychart_	= nullptr;
    bool			showtools_;
    Interval<float>		primaryzrange_	= Interval<float>::udf();
    int				selected_	= -1;
    Interval<float>		syncrange_	= Interval<float>::udf();
    int				syncview_	= -1;
    bool			alllocked_	= false;

    void			addViewer(int);
    void			addTools(int);
    void			selectView(int);

    void			trackaddCB(CallBacker*);
    void			trackremoveCB(CallBacker*);
    void			colSelectCB(CallBacker*);
    void			selectCB(CallBacker*);
    void			syncViewsCB(CallBacker*);
    void			zRangeChangeCB(CallBacker*);
};
