#pragma once
/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uicharttoolsmod.h"

#include "uigroup.h"


mExpClass(uiChartTools) uiLogMergeGrp : public uiGroup
{
public:
				uiLogMergeGrp(uiParent*);
				~uiLogMergeGrp();

protected:

    uiLogView*			logdisp_		= nullptr;
    uiLogMergerPropDlg*		propdlg_		= nullptr;
};


mExpClass(uiChartTools) uiMergeLogsGrp : public uiGroup
{
mODTextTranslationClass(uiMergeLogsGrp)
public:
			uiMergeLogsGrp(uiParent*, const Interval<float>&,
				       const BufferString&,
				       const OD::LineStyle&,
				       const BufferStringSet&,
				       const TypeSet<OD::LineStyle>&);
			~uiMergeLogsGrp();

    OD::LineStyle	getOutLineStyle() const;
    OD::LineStyle	getLineStyle(const char*) const;
    Interval<float>	getRange() const;

    Notifier<uiMergeLogsGrp>	changed;

protected:
    uiGenInput*			rangefld_;
    uiSelLineStyle*		outls_;
    BufferStringSet		lognms_;
    ObjectSet<uiSelLineStyle>	linestyles_;

    void			changedCB(CallBacker*);

};


mExpClass(uiChartTools) uiLogMergerPropDlg : public uiDialog
{
mODTextTranslationClass(uiLogMergerPropDlg)
public:
			uiLogMergerPropDlg(uiParent*, uiLogChart*,
					   const Interval<float>&,
					   const BufferString&,
					   const OD::LineStyle&,
					   const BufferStringSet&,
					   const TypeSet<OD::LineStyle>&);
			~uiLogMergerPropDlg();

    OD::LineStyle	getOutLineStyle() const;
    OD::LineStyle	getLineStyle(const char*) const;
    Interval<float>	getRange() const;

    Notifier<uiLogMergerPropDlg>	changed;

protected:
    uiLogChart*		logchart_;
    uiTabStack*		tabs_;
    uiMergeLogsGrp*	logsgrp_;
    uiLogChartGrp*	chartgrp_;

    void		changedCB(CallBacker*);
};
