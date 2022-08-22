#pragma once
/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
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
class uiTabStack;

namespace OD { class LineStyle; }

mExpClass(uiWellCharts) uiGridStyleGrp : public uiGroup
{ mODTextTranslationClass(uiGridStyleGrp)
public:
			uiGridStyleGrp(uiParent*,const uiString&,
					bool ticksbycount);
			~uiGridStyleGrp();

    bool		isVisible() const;
    void		setVisible(bool);
    void		setStepSensitive(bool);

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


mExpClass(uiWellCharts) uiLogChartGrp : public uiGroup
{ mODTextTranslationClass(uiLogChartGrp)
public:
			uiLogChartGrp(uiParent*, uiLogChart*);
			~uiLogChartGrp();

    void		update();
    void		setLogChart(uiLogChart*);

protected:
    uiLogChart*		logchart_;
    uiColorInput*	bgcolorfld_;
    uiGenInput*		scalefld_;
    uiGridStyleGrp*	majorzgridfld_;
    uiGridStyleGrp*	minorzgridfld_;
    uiGridStyleGrp*	majorloggridfld_;
    uiGridStyleGrp*	minorloggridfld_;

    void		bgColorChgCB(CallBacker*);
    void		zgridChgCB(CallBacker*);
    void		lgridChgCB(CallBacker*);
    void		scaleChgCB(CallBacker*);
};


mExpClass(uiWellCharts) uiLogsGrp : public uiGroup
{ mODTextTranslationClass(uiLogsGrp)
public:
			uiLogsGrp(uiParent*, uiLogChart*);
			~uiLogsGrp();

    void		update();
    void		setLogChart(uiLogChart*);

protected:
    uiLogChart*		logchart_;
    uiListBox*		logselfld_;
    uiLogCurveProps*	logpropfld_;

    void		logselCB(CallBacker*);
};


mExpClass(uiWellCharts) uiMarkersGrp : public uiGroup
{ mODTextTranslationClass(uiMarkersGrp)
public:
			uiMarkersGrp(uiParent*, uiLogChart*);
			~uiMarkersGrp();

    void		update();
    void		setLogChart(uiLogChart*);

protected:
    uiLogChart*		logchart_;
    uiListBox*		markerselfld_;
    uiSelLineStyle*	markerlinefld_;

    void		markerselCB(CallBacker*);
    void		lineStyleChgCB(CallBacker*);
};


mExpClass(uiWellCharts) uiLogViewPropDlg : public uiDialog
{ mODTextTranslationClass(uiLogViewPropDlg)
public:
			uiLogViewPropDlg(uiParent*, uiLogChart*,
					 bool withapply=false);
			~uiLogViewPropDlg();

    void		setLogChart(uiLogChart*);
    IOPar		getCurrentSettings() const;

protected:
    uiLogChart*		logchart_;
    uiTabStack*		tabs_;
    uiLogChartGrp*	chartgrp_;
    uiLogsGrp*		logsgrp_;
    uiMarkersGrp*	markersgrp_;
    IOPar		settingsbackup_;
    bool		withapply_ = false;

    void		applyCB(CallBacker*);
    void		updateLogsCB(CallBacker*);
    void		updateMarkersCB(CallBacker*);
    void		restoreCB(CallBacker*);
    void		closeCB(CallBacker*);
    bool		rejectOK(CallBacker*) override;
};
