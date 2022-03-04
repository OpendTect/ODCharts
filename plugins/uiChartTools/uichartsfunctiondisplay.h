#pragma once

/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:        Nanne Hemstra
 Date:          January 2022
________________________________________________________________________

-*/

#include "uicharttoolsmod.h"

#include "uichartview.h"
#include "uifuncdispbase.h"
#include "uifuncdrawerbase.h"
#include "uiaxishandlerbase.h"

class uiBorder;
class uiChartAxis;
class uiChartSeries;
class uiLineSeries;
class uiAreaSeries;
class uiScatterSeries;


mExpClass(uiChartTools) uiChartsAxisHandler : public uiAxisHandlerBase
{
public:
			uiChartsAxisHandler(uiChartView*,const Setup&);
			~uiChartsAxisHandler();

    void		setCaption(const uiString&) override;
    uiString		getCaption() const override;

    void		setBorder(const uiBorder&) override;
    void		setIsLog(bool yn) override;

    void		setRange(const StepInterval<float>&,
				 float* axst=nullptr) override;
    void		setBounds(Interval<float>) override;

    StepInterval<float> range() const override;

    void		setAxisStyle();

    uiChartAxis*	axis()		{ return axis_; }

protected:
    uiChartView*	chartview_ = nullptr;
    uiChartAxis*	axis_ = nullptr;

    void		makeAxis();

};


mExpClass(uiChartsTools) uiChartsFunctionDisplay : public uiFuncDispBase
						 , public uiChartView
{
mODTextTranslationClass(uiChartsFunctionDisplay)
public:
			uiChartsFunctionDisplay(uiParent*,const Setup&);
			~uiChartsFunctionDisplay();

    void		setTitle(const uiString&) override;

    Geom::PointF	mapToPosition(const Geom::PointF&,
				      bool y2=false) override;
    Geom::PointF	mapToValue(const Geom::PointF&,
				   bool y2=false) override;

    void		draw() override;
    uiObject*		uiobj()			{ return this; }

    const NotifierAccess&	mouseMoveNotifier() override
				{ return uiChartView::mouseMove; }

    uiChartsAxisHandler*	xAxis() const;
    uiChartsAxisHandler*	yAxis(bool y2) const;

    void		setSeriesStyle();
    void		setChartStyle();

    Notifier<uiChartsFunctionDisplay>	pointSelected;
    Notifier<uiChartsFunctionDisplay>	pointChanged;

protected:
    uiLineSeries*	yseries_		= nullptr;
    uiLineSeries*	y2series_		= nullptr;
    uiAreaSeries*	yarea_			= nullptr;
    uiAreaSeries*	y2area_			= nullptr;
    uiScatterSeries*	selseries_		= nullptr;

    uiChartSeries*	getSeries(bool);

    void		cleanUp() override;
    void		drawMarkLines() override;

    void		makeSeries();

    void		mouseMoveCB(CallBacker*) override;
    void		mousePressCB(CallBacker*) override;
    void		mouseReleaseCB(CallBacker*) override;
    void		pointSelectedCB(CallBacker*);

    bool		setSelPt(const Geom::PointF&);
};


mExpClass(uiChartTools) uiChartsFunctionDrawer : public uiFuncDrawerBase
					       , public uiChartView
{
public:
			uiChartsFunctionDrawer(uiParent*,const Setup&);
			~uiChartsFunctionDrawer();

    void		draw(CallBacker*) override;
    uiObject*		uiobj() override 	{ return this; }

    uiChartsAxisHandler*	xAxis() const;
    uiChartsAxisHandler*	yAxis() const;

protected:

};
