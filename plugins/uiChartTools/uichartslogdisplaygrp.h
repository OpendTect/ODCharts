#pragma once
/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uicharttoolsmod.h"

#include "uidialog.h"
#include "uiwelldisplayserver.h"
#include "draw.h"

class uiGenInput;
class uiLogChart;
class uiLogChartGrp;
class uiLogDisplayPropDlg;
class uiLogView;
class uiSelLineStyle;
class uiTabStack;
class uiToolButton;
namespace Well { class Log; }


mExpClass(uiChartTools) uiChartsLogDisplayGrp : public uiLogDisplayGrp
{
mODTextTranslationClass(uiChartsLogDisplayGrp)
public:
			uiChartsLogDisplayGrp(uiParent*);
			~uiChartsLogDisplayGrp();

    void		setLogs(const ObjectSet<LogSelection>&) override;
    void		update() override;

protected:

    void			changeWellButPush(CallBacker*);
    void			showSettingsCB(CallBacker*);
    void			settingsChgCB(CallBacker*);

    int				wellidx_		= 0;
    ObjectSet<LogSelection>	logdatas_;
    Interval<float>		disprange_		=Interval<float>::udf();
    TypeSet<OD::LineStyle>	ls_;

    uiLogView*			logdisp_		= nullptr;
    uiLogDisplayPropDlg*	propdlg_		= nullptr;
    uiToolButton*		nextlog_		= nullptr;
    uiToolButton*		prevlog_		= nullptr;
};


mExpClass(uiChartTools) uiDisplayLogsGrp : public uiGroup
{
mODTextTranslationClass(uiDisplayLogsGrp)
public:
			uiDisplayLogsGrp(uiParent*,const Interval<float>&,
					const BufferStringSet&,
					const TypeSet<OD::LineStyle>&);
			~uiDisplayLogsGrp();

    OD::LineStyle	getLineStyle(const char*) const;
    Interval<float>	getRange() const;

    Notifier<uiDisplayLogsGrp>	changed;

protected:
    uiGenInput*			rangefld_;
    BufferStringSet		lognms_;
    ObjectSet<uiSelLineStyle>	linestyles_;

    void			changedCB(CallBacker*);
};


mExpClass(uiChartTools) uiLogDisplayPropDlg : public uiDialog
{
mODTextTranslationClass(uiLogDisplayPropDlg)
public:
			uiLogDisplayPropDlg(uiParent*,uiLogChart*,
					   const Interval<float>&,
					   const BufferStringSet&,
					   const TypeSet<OD::LineStyle>&);
			~uiLogDisplayPropDlg();

    OD::LineStyle	getLineStyle(const char*) const;
    Interval<float>	getRange() const;

    Notifier<uiLogDisplayPropDlg>	changed;

protected:
    uiLogChart*		logchart_;
    uiTabStack*		tabs_;
    uiDisplayLogsGrp*	logsgrp_;
    uiLogChartGrp*	chartgrp_;

    void		changedCB(CallBacker*);
};
