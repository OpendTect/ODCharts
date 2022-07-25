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
#include "wellextractdata.h"

#include "menuhandler.h"

class MnemonicSelection;
class uiCheckBox;
class uiComboBox;
class uiLogChart;
class uiLogView;
class uiLogViewPropDlg;
class uiLogViewTable;
class uiLogViewerTree;
class uiSpinBox;
class uiTabStack;
class uiToolBar;
class uiToolButton;
class uiWellFilterGrp;
class uiZRangeSelect;

namespace Well { class Data; }


mExpClass(uiWellCharts) uiLogViewWinBase : public uiDialog
{ mODTextTranslationClass(uiLogViewBase)
public:
    virtual		~uiLogViewWinBase();

    void		addWellData(const DBKeySet&,
				    const ManagedObjectSet<TypeSet<int>>&);
    void		addWellData(const DBKeySet&,
				    const ManagedObjectSet<TypeSet<int>>&,
				    const BufferStringSet& mrknms);
    void		addWellData(const DBKeySet&,
				    const BufferStringSet& lognms,
				    const BufferStringSet& mrknms);

    void		setCurrentView(int);
    virtual void	loadFile(const char*);
    virtual void	saveFile(const char*);
    virtual void	loadWells(const BufferStringSet& wellids,
				  const BufferStringSet& logids)	{}
    virtual void	setSelected(const DBKeySet&,
				    const BufferStringSet& lognms,
				    const BufferStringSet& mrknms,
				    bool show=true)	{}
    virtual void	setSelected(const DBKeySet&,
				    const MnemonicSelection& mns,
				    const BufferStringSet& mrknms,
				    bool show=true)	{}

    static const char*	defDirStr()	{ return "WellInfo"; }
    static const char*	extStr()	{ return "lvpar"; }
    static const char*	filtStr()	{ return "*.lvpar"; }

    Notifier<uiLogViewWinBase>	showFilter;

protected:
			uiLogViewWinBase(uiParent*,int nrcol=0,
					 bool showtools=true,
					 bool showFilter=false);

    uiToolBar*		tb_ = nullptr;
    uiLogViewTable*	logviewtbl_;

    BufferString	filename_;
    bool		needsave_ = false;
    uiComboBox*		zdomainfld_;

    MenuItem		newitem_;
    MenuItem		openitem_;
    MenuItem		saveitem_;
    MenuItem		saveasitem_;
    bool		showfilter_;

    bool		checkSave();
    void		createToolBar();
    virtual void	addApplicationToolBar()	{}
    virtual void	clearAll();

    virtual void	uiInitCB(CallBacker*);
    void		filterCB(CallBacker*);
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
					   const BufferStringSet& markernms,
					   bool showfilter=false);
			uiLockedLogViewWin(uiParent*,
					   const ObjectSet<Well::Data>&,
					   const MnemonicSelection& mns,
					   const BufferStringSet& markernms,
					   bool showfilter=false);
			~uiLockedLogViewWin();

    void		setSelected(const DBKeySet&,
				    const BufferStringSet& lognms,
				    const BufferStringSet& mrkrs,
				    bool show=true) override;
    void		setSelected(const DBKeySet&,
				    const MnemonicSelection& mns,
				    const BufferStringSet& mrkrs,
				    bool show=true) override;
    void		loadFile(const char*) override;

protected:
    uiWellFilterGrp*	logfiltergrp_;
    uiLogViewPropDlg*	propdlg_ = nullptr;
    uiCheckBox*		flattenfld_;
    uiComboBox*		markerfld_;
    uiZRangeSelect*	zrangeselfld_;

    MenuItem		settingsbuttonitem_;
    MenuItem		unzoombuttonitem_;

    DBKeySet		selwells_;
    BufferStringSet	sellogs_;
    BufferStringSet	selmrkrs_;
    Well::ZRangeSelector selzrng_;

    void		addApplicationToolBar() override;
    void		closePropertiesDlg();

    void		initCB(CallBacker*);
    uiObject*		initCommonUI(uiGroup*);
    void		chartChgCB(CallBacker*);
    void		dataChgCB(CallBacker*);
    void 		flattenChgCB(CallBacker*);
    void		showSettingsCB(CallBacker*);
    void		applySettingsCB(CallBacker*);
    void		updateMarkersCB(CallBacker*);
    void		zoomRangeCB(CallBacker*);
    void		zoomResetCB(CallBacker*);
    void		zdomainChgCB(CallBacker*) override;
};


mExpClass(uiWellCharts) uiLogViewWin : public uiLogViewWinBase
{ mODTextTranslationClass(uiLogViewWin)
public:
			uiLogViewWin(uiParent*,int nrcol=0);
			~uiLogViewWin();

    void		addWell(const DBKey&,const TypeSet<int>& logs);
    void		addWell(int,const DBKey&,const TypeSet<int>& logs);
    void		loadWells(const BufferStringSet& wellids,
				  const BufferStringSet& logids) override;

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
