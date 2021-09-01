#pragma once
/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		November 2020
________________________________________________________________________

-*/

#include "uichartsmod.h"

#include "uiobj.h"

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

    Notifier<uiChartView>	doubleClick;

protected:
    ODChartView&		mkbody(uiParent*,const char*);
    ODChartView*		odchartview_;

    uiChart*			uichart_;
};
