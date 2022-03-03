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
class uiLogViewPropDlg;
class uiTabStack;
class uiToolBar;
class uiToolButton;
class uiWellFilterGrp;
namespace Well { class Data; };

mExpClass(uiWellCharts) uiLogViewWinBase : public uiDialog
{ mODTextTranslationClass(uiLogViewBase)
public:
			uiLogViewWinBase(uiParent*,int nrcol=0,
					 bool showtools=true);
    virtual		~uiLogViewWinBase();

    void		addWellData(const DBKeySet&,
				    const ManagedObjectSet<TypeSet<int>>&);
    void		addWellData(const DBKeySet&,
				    const BufferStringSet& lognms,
				    const BufferStringSet& mrknms);

    void		setCurrentView(int);
    virtual void	loadFile(const char*)		{}
    virtual void	saveFile(const char*)		{}
    virtual void	loadWells(const BufferStringSet& wellids,
				  const BufferStringSet& logids)	{}
    virtual void	setSelected(const DBKeySet&,
				    const BufferStringSet& lognms,
				    const BufferStringSet& mrknms)	{}

    static const char*	defDirStr()	{ return "WellInfo"; }
    static const char*	extStr()	{ return "lvpar"; }
    static const char*	filtStr()	{ return "*.lvpar"; }

protected:
    uiToolBar*		tb_ = nullptr;
    uiLogViewTable*	logviewtbl_;

    BufferString	filename_;
    bool		needsave_ = false;
    uiComboBox*		zdomainfld_;

    MenuItem		newitem_;
    MenuItem		openitem_;
    MenuItem		saveitem_;
    MenuItem		saveasitem_;

    bool		checkSave();
    void		createToolBar();
    virtual void	addApplicationToolBar()	{}
    virtual void	clearAll();

    virtual void	uiInitCB(CallBacker*);
    virtual void	newCB(CallBacker*);
    virtual void	openCB(CallBacker*);
    virtual void	saveCB(CallBacker*);
    virtual void	saveasCB(CallBacker*);
    virtual void	closeCB(CallBacker*);
    virtual void	selTrackChgCB(CallBacker*) 		{}
    virtual void	zdomainChgCB(CallBacker*);

};


mExpClass(uiWellCharts) uiLockedLogViewWin : public uiLogViewWinBase
{ mODTextTranslationClass(uiLockedLogViewWin)
public:
			uiLockedLogViewWin(uiParent*,
					   const ObjectSet<Well::Data>&,
					   const BufferStringSet& lognms,
					   const BufferStringSet& markernms);
			~uiLockedLogViewWin();

    void		setSelected(const DBKeySet&,
				    const BufferStringSet& lognms,
				    const BufferStringSet& mrkrs) override;
protected:
    uiWellFilterGrp*	logfiltergrp_;
    uiLogViewPropDlg*	propdlg_ = nullptr;

    MenuItem		settingsbuttonitem_;
    MenuItem		unzoombuttonitem_;

    void		addApplicationToolBar() override;

    void		chartChgCB(CallBacker*);
    void		dataChgCB(CallBacker*);
    void		showSettingsCB(CallBacker*);
    void		applySettingsCB(CallBacker*);
    void		zoomResetCB(CallBacker*);
};


mExpClass(uiWellCharts) uiLogViewWin : public uiLogViewWinBase
{ mODTextTranslationClass(uiLogViewWin)
public:
			uiLogViewWin(uiParent*,int nrcol=0);
			~uiLogViewWin();

    void		addWell(const DBKey&,const TypeSet<int>& logs);
    void		addWell(int,const DBKey&,const TypeSet<int>& logs);
    void		loadFile(const char*) override;
    void		loadWells(const BufferStringSet& wellids,
				  const BufferStringSet& logids) override;
    void		saveFile(const char*) override;

protected:
    uiLogViewerTree*	logviewtree_;

    MenuItem		addbuttonitem_;
    MenuItem		rmvbuttonitem_;

    void		addApplicationToolBar() override;

    void		clearAll() override;
    void		add(int,const MultiID&,const BufferStringSet&);
    void		addLog(int,const MultiID&,const BufferString&);
    void		rmvLog(int,const MultiID&,const BufferString&);

    void		addMarker(int,const MultiID&,const BufferString&);
    void		rmMarker(int,const MultiID&,const BufferString&);

    void		uiInitCB(CallBacker*) override;
    void		addLogCB(CallBacker*);
    void		removeLogCB(CallBacker*);
    void		addMarkerCB(CallBacker*);
    void		removeMarkerCB(CallBacker*);
    void		addTrackCB(CallBacker*);
    void		rmvTrackCB(CallBacker*);
    void		selTrackChgCB(CallBacker*) override;
};
