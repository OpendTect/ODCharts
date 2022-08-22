#pragma once
/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uiwellchartsmod.h"
#include "uigroup.h"

#include "bufstring.h"
#include "multiid.h"
#include "uistring.h"


class uiTreeView;
class uiTreeViewItem;

typedef std::pair<MultiID,BufferString> LogID;
typedef std::pair<MultiID,BufferString> MarkerID;

mExpClass(uiWellCharts) uiLogViewerTree : public uiGroup
{ mODTextTranslationClass(uiLogViewerTree)
public:
			uiLogViewerTree(uiParent*);
			~uiLogViewerTree();

    void		checkLogsFor(const MultiID&,const BufferStringSet&);
    void		checkMarkersFor(const MultiID&,const BufferStringSet&);
    void		uncheckAll();
    void		collapseAll();

    CNotifier<uiLogViewerTree,const LogID&> logChecked;
    CNotifier<uiLogViewerTree,const LogID&> logUnchecked;
    CNotifier<uiLogViewerTree,const MarkerID&> markerChecked;
    CNotifier<uiLogViewerTree,const MarkerID&> markerUnchecked;

protected:
    uiTreeView*		treeview_;

    void		init();
    void		addWells();
    void		addLogs(const MultiID&,uiTreeViewItem*);
    void		addMarkers(const MultiID&,uiTreeViewItem*);

    void		logStateChgCB(CallBacker*);
    void		markerStateChgCB(CallBacker*);
    void		allMarkerStateChgCB(CallBacker*);
    void		wellExpandCB(CallBacker*);
};
