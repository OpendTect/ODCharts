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
class uiLogViewPropDlg;
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

    void		addLogSelection(const ObjectSet<Well::SubSelData>&)
								override;
    void		update() override;
    void		setDisplayProps(const Well::DisplayProperties::Log&,
					bool forname=true) override;

protected:

    void			changeWellButPush(CallBacker*);
    void			showSettingsCB(CallBacker*);

    int				wellidx_		= 0;
    ObjectSet<Well::SubSelData>	logdatas_;
    StringPairSet		specstyles_;
    OD::LineStyle		normstyle_;
    Interval<float>		disprange_		=Interval<float>::udf();

    uiLogView*			logdisp_		= nullptr;
    uiLogViewPropDlg*		propdlg_		= nullptr;
    uiToolButton*		nextlog_		= nullptr;
    uiToolButton*		prevlog_		= nullptr;
};
