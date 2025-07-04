#pragma once
/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
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
class uiXYChartSeries;
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


mExpClass(uiChartTools) uiChartsFunctionDisplay : public uiFuncDispBase
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
    uiObject*		uiobj() override		{ return this; }

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


mExpClass(uiChartTools) uiChartsMultiFunctionDisplay
						: public uiMultiFuncDispBase
						, public uiChartView
{
mODTextTranslationClass(uiChartsMultiFunctionDisplay)
public:
			uiChartsMultiFunctionDisplay(uiParent*,const Setup&);
			~uiChartsMultiFunctionDisplay();

    void		setTitle(const uiString&) override;
    void		setEmpty() override;

    Geom::PointF	mapToPosition(const Geom::PointF&) override;
    Geom::PointF	mapToValue(const Geom::PointF&) override;

    void		draw() override;
    uiObject*		uiobj() override		{ return this; }

    uiChartsAxisHandler*	xAxis() const;
    uiChartsAxisHandler*	yAxis() const;

    void		setChartStyle();
    void		setSeriesStyle();

    void		addFunction(FunctionPlotData*) override;
    void		removeFunction(const char* nm) override;
    void		setVisible(const char* nm,bool) override;

protected:

    ObjectSet<uiXYChartSeries>	series_;

    uiXYChartSeries*	getSeries(int);

    void		makeSeries(const FunctionPlotData&);
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
