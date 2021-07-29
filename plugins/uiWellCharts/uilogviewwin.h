#pragma once
/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		November 2020
________________________________________________________________________

-*/

#include "uiwellchartsmod.h"
#include "uimainwin.h"
#include "menuhandler.h"


class uiLogChart;
class uiLogView;
class uiLogViewTable;
class uiToolBar;
class uiToolButton;
class uiWellInputGroup;

mExpClass(uiWellCharts) uiLogViewWin : public uiMainWin
{ mODTextTranslationClass(uiLogViewWin)
public:
				uiLogViewWin(uiParent*);
				~uiLogViewWin();

    static const char*		defDirStr()	{ return "WellInfo"; }
    static const char*		extStr()	{ return "lvpar"; }
    static const char*		filtStr()	{ return "*.lvpar"; }

protected:
    uiToolBar*			tb_ = nullptr;
    uiWellInputGroup*		inputgrp_;
    uiLogViewTable*		logviewtbl_;

    BufferString		filename_;
    bool			needsave_ = false;

    MenuItem			newitem_;
    MenuItem			openitem_;
    MenuItem			saveitem_;
    MenuItem			saveasitem_;
    MenuItem			setbuttonitem_;
    MenuItem			addbuttonitem_;
    MenuItem			rmvbuttonitem_;

    bool			checkSave();
    void			clearAll();
    void			createToolBar();
    void			add(int,const MultiID&,const BufferStringSet&);

    void			uiInitCB(CallBacker*);
    void			newCB(CallBacker*);
    void			openCB(CallBacker*);
    void			saveCB(CallBacker*);
    void			saveasCB(CallBacker*);
    void			addCB(CallBacker*);
    void			addTrackCB(CallBacker*);
    void			rmvTrackCB(CallBacker*);
    void			closeCB(CallBacker*);
};
