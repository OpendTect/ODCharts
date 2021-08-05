#pragma once
/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		August 2021
________________________________________________________________________

-*/

#include "uiwellchartsmod.h"
#include "uigroup.h"

class uiLogView;


mClass(uiWellCharts) uiLogViewToolGrp : public uiGroup
{ mODTextTranslationClass(uiLogViewToolGrp)
public:
			uiLogViewToolGrp(uiParent*, uiLogView&);
			~uiLogViewToolGrp();

    int			addButton(const char* icon, const uiString& tooltip,
				  const CallBack& cb=CallBack(),
				  bool toggle=false, int id=-1);
    void		addObject(uiObject*);

    bool		isLocked() const;

protected:
    uiLogView&		logview_;
    ObjectSet<uiObject>	addedobjects_;

    void		lockCB(CallBacker*);
};
