#pragma once
/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		July 2021
________________________________________________________________________

-*/

#include "uiwellchartsmod.h"
#include "uidialog.h"
#include "uigroup.h"

class uiColorInput;
class uiGenInput;
class uiListBox;
class uiLogChart;
class uiLogCurveProps;
class uiSelLineStyle;

namespace OD { class LineStyle; }

mClass(uiWellCharts) uiGridStyleGrp : public uiGroup
{ mODTextTranslationClass(uiGridStyleGrp)
public:
			uiGridStyleGrp(uiParent*,const uiString&,
					bool ticksbycount);
			~uiGridStyleGrp();

    bool		isVisible() const;
    void		setVisible(bool);

    void		setStyle(const OD::LineStyle&);
    OD::LineStyle	getStyle() const;

    void		setSteps(int);
    int			getSteps() const;

    Notifier<uiGridStyleGrp>	changed;

protected:
    uiSelLineStyle*	linestylefld_;
    uiGenInput*		stepsfld_;
    bool		ticksbycount_;

    void		stepChgCB(CallBacker*);
    void		stepCheckedCB(CallBacker*);
    void		lineStyleChgCB(CallBacker*);
};


mClass(uiWellCharts) uiLogChartGrp : public uiGroup
{ mODTextTranslationClass(uiLogChartGrp)
public:
			uiLogChartGrp(uiParent*,uiLogChart&);
			~uiLogChartGrp();

protected:
    uiLogChart&		logchart_;
    uiColorInput*	bgcolorfld_;
    uiGridStyleGrp*	majorzgridfld_;
    uiGridStyleGrp*	minorzgridfld_;
    uiGridStyleGrp*	majorloggridfld_;
    uiGridStyleGrp*	minorloggridfld_;

    void		bgColorChgCB(CallBacker*);
    void		zgridChgCB(CallBacker*);
    void		lgridChgCB(CallBacker*);
};


mClass(uiWellCharts) uiLogsGrp : public uiGroup
{ mODTextTranslationClass(uiLogsGrp)
public:
			uiLogsGrp(uiParent*,uiLogChart&);
			~uiLogsGrp();

    void		update();

protected:
    uiLogChart&		logchart_;
    uiListBox*		logselfld_;
    uiLogCurveProps*	logpropfld_;

    void		logselCB(CallBacker*);
};


mClass(uiWellCharts) uiLogViewPropDlg : public uiDialog
{ mODTextTranslationClass(uiLogViewPropDlg)
public:
			uiLogViewPropDlg(uiParent*, uiLogChart&);
			~uiLogViewPropDlg();

protected:
    uiLogChart&		logchart_;
    uiLogChartGrp*	chartgrp_;
    uiLogsGrp*		logsgrp_;

    void		updateCB(CallBacker*);
};
