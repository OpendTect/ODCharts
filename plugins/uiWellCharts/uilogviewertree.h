#pragma once
/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		August 2021
________________________________________________________________________

-*/

#include "uiwellchartsmod.h"
#include "uitreeview.h"

#include "bufstring.h"
#include "multiid.h"
#include "uistring.h"


class uiTreeViewItem;

typedef std::pair<MultiID,BufferString> LogID;
typedef std::pair<MultiID,BufferString> MarkerID;

mExpClass(uiWellCharts) uiLogViewerTree : public uiTreeView
{ mODTextTranslationClass(uiLogViewerTree)
public:
			uiLogViewerTree(uiParent*);
			~uiLogViewerTree();

    void		checkLogsFor(const MultiID&,const BufferStringSet&);
    void		checkMarkersFor(const MultiID&,const BufferStringSet&);

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
};
