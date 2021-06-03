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

class uiGroup;
class uiLogView;
class uiToolButton;
class uiWellInputGroup;

mExpClass(uiWellCharts) uiLogViewWin : public uiMainWin
{ mODTextTranslationClass(uiLogViewWin);
public:
				uiLogViewWin(uiParent*);
				~uiLogViewWin();

protected:

    uiWellInputGroup*		inputgrp_;

    uiGroup*			viewgrp_;
    ObjectSet<uiLogView>	logviews_;

    void			addCB(CallBacker*);
    void			addViewer();
};
