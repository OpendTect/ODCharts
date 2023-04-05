#pragma once
/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uiwellchartsmod.h"
#include "uigroup.h"

#include "uichartfillx.h"

class LogCurve;
class uiGenInput;
class uiSelLineStyle;
class uiColorInput;
class uiColorTableGroup;
class uiLogChart;

mExpClass(uiWellCharts) uiLogFillProps : public uiGroup
{ mODTextTranslationClass(uiLogFillProps)
public:
			uiLogFillProps(uiParent*,uiChartFillx::FillDir,
				       uiLogChart*);
			~uiLogFillProps();

    void		setLogFill(int);
    void		setLogChart(uiLogChart*);

protected:
    uiLogChart*			logchart_ = nullptr;
    LogCurve*			logcurve_ = nullptr;
    uiChartFillx*		fill_ = nullptr;
    uiChartFillx::FillDir	filldir_;

    uiGenInput*		filltypefld_;
    uiGenInput*		filllimitfld_;
    uiColorInput*	fillcolorfld_;
    uiColorTableGroup*	fillgradientfld_;
    uiGenInput*		gradientlogfld_;
    uiGenInput*		fillbasefld_;
    uiGenInput*		fillseriesfld_;

    void		fillTypeChgCB(CallBacker*);
    void		fillLimitChgCB(CallBacker*);
    void		fillColorChgCB(CallBacker*);
    void		fillGradientChgCB(CallBacker*);
    void		fillGradientLogChgCB(CallBacker*);
    void		fillBaseChgCB(CallBacker*);
    void		fillSeriesChgCB(CallBacker*);
};


mExpClass(uiWellCharts) uiLogCurveProps : public uiGroup
{ mODTextTranslationClass(uiLogCurveProps)
public:
    enum FillLimit	{ Track, Baseline, Curve };
			mDeclareEnumUtils(FillLimit)

			uiLogCurveProps(uiParent*,uiLogChart*);
			~uiLogCurveProps();

    void		setLogCurve(int);
    void		setLogChart(uiLogChart*);
    void		update();

    CNotifier<uiLogCurveProps,const Interval<float>&>	rangechanged;

protected:
    uiLogChart*		logchart_ = nullptr;
    LogCurve*		logcurve_ = nullptr;

    uiGenInput*		rangefld_;
    uiSelLineStyle*	linestylefld_;
    uiLogFillProps*	leftfillfld_;
    uiLogFillProps*	rightfillfld_;

    void		lineStyleChgCB(CallBacker*);
    void		rangeChgCB(CallBacker*);
};
