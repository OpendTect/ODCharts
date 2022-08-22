#pragma once
/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uiwellchartsmod.h"
#include "uigroup.h"

class uiLogView;


mExpClass(uiWellCharts) uiLogViewToolGrp : public uiGroup
{ mODTextTranslationClass(uiLogViewToolGrp)
public:
			uiLogViewToolGrp(uiParent*,uiLogView&);
			~uiLogViewToolGrp();

    int			addButton(const char* icon,const uiString& tooltip,
				  const CallBack& cb=CallBack(),
				  bool toggle=false,int id=-1);
    void		addObject(uiObject*);

    bool		isLocked() const;
    void		setLocked(bool yn=true);

protected:
    uiLogView&		logview_;
    ObjectSet<uiObject>	addedobjects_;

    void		lockCB(CallBacker*);
};
