#pragma once
/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		November 2020
________________________________________________________________________

-*/

#include "uiwellchartsmod.h"
#include "uidialog.h"

#include "menuhandler.h"

class uiComboBox;
class uiLogChart;
class uiLogView;
class uiLogViewTable;
class uiLogViewerTree;
class uiToolBar;
class uiToolButton;

mExpClass(uiWellCharts) uiLogViewWin : public uiDialog
{ mODTextTranslationClass(uiLogViewWin)
public:
			uiLogViewWin(uiParent*,int nrcol=0);
			~uiLogViewWin();

    void		setCurrentView(int);
    void		loadFile(const char*);
    void		loadWells(const BufferStringSet& wellids,
				  const BufferStringSet& logids);
    void		addWellData(const DBKeySet&,
				    const ManagedObjectSet<TypeSet<int>>&);
    void		addWellData(const DBKey&,const TypeSet<int>& logs);
    void		addWellData(int,const DBKey&,const TypeSet<int>& logs);

    static const char*	defDirStr()	{ return "WellInfo"; }
    static const char*	extStr()	{ return "lvpar"; }
    static const char*	filtStr()	{ return "*.lvpar"; }

protected:
    uiToolBar*		tb_ = nullptr;
    uiLogViewerTree*	logviewtree_;
    uiLogViewTable*	logviewtbl_;

    BufferString	filename_;
    bool		needsave_ = false;

    MenuItem		newitem_;
    MenuItem		openitem_;
    MenuItem		saveitem_;
    MenuItem		saveasitem_;
    MenuItem		addbuttonitem_;
    MenuItem		rmvbuttonitem_;
    uiComboBox*		zdomainfld_;

    bool		checkSave();
    void		clearAll();
    void		createToolBar();
    void		add(int,const MultiID&,const BufferStringSet&);
    void		addLog(int,const MultiID&,const BufferString&);
    void		rmvLog(int,const MultiID&,const BufferString&);

    void		addMarker(int,const MultiID&,const BufferString&);
    void		rmMarker(int,const MultiID&,const BufferString&);

    void		uiInitCB(CallBacker*);
    void		newCB(CallBacker*);
    void		openCB(CallBacker*);
    void		saveCB(CallBacker*);
    void		saveasCB(CallBacker*);
    void		addLogCB(CallBacker*);
    void		removeLogCB(CallBacker*);
    void		addMarkerCB(CallBacker*);
    void		removeMarkerCB(CallBacker*);
    void		addTrackCB(CallBacker*);
    void		rmvTrackCB(CallBacker*);
    void		closeCB(CallBacker*);
    void		selTrackChgCB(CallBacker*);
    void		zdomainChgCB(CallBacker*);
};
