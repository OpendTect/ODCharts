#pragma once
/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		July 2021
________________________________________________________________________

-*/

#include "uiwellchartsmod.h"
#include "uigroup.h"

class LogCurve;
class uiGenInput;
class uiSelLineStyle;

mClass(uiWellCharts) uiLogCurveProps : public uiGroup
{ mODTextTranslationClass(uiLogCurveProps)
public:
			uiLogCurveProps(uiParent*);
			uiLogCurveProps(uiParent*,LogCurve*);
			~uiLogCurveProps();

    void		setLogCurve(LogCurve*);

protected:
    LogCurve*		logcurve_ = nullptr;

    uiGenInput*		rangefld_;
    uiSelLineStyle*	linestylefld_;

    void		lineStyleChgCB(CallBacker*);
    void		rangeChgCB(CallBacker*);
};
