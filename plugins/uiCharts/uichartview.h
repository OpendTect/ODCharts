#pragma once
/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uichartsmod.h"

#include "uiobj.h"
#include "keyenum.h"

class ODChartView;
class uiChart;

mExpClass(uiCharts) uiChartView : public uiObject
{
public:
				uiChartView(uiParent*,const char* nm);
				~uiChartView();

    uiChart*			chart()		{ return uichart_; }
    void			setChart(uiChart*);

    enum ZoomStyle		{ NoZoom, VerticalZoom, HorizontalZoom,
				  RectangleZoom };
    void			setBackgroundColor(const OD::Color&) override;
    void			setMinimumSize(int width,int height);
    void			setZoomStyle(ZoomStyle);
    OD::ButtonState		mouseButton() const;
    OD::ButtonState		keyModifier() const;

    Notifier<uiChartView>	doubleClick;
    CNotifier<uiChartView,const Geom::PointF&>	mouseMove;
    CNotifier<uiChartView,const Geom::PointF&>	mousePress;
    CNotifier<uiChartView,const Geom::PointF&>	mouseRelease;

protected:
    ODChartView&		mkbody(uiParent*,const char*);
    ODChartView*		odchartview_;

    uiChart*			uichart_;
};
