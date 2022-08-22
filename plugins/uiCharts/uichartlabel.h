#pragma once
/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uichartsmod.h"

#include "callback.h"
#include "draw.h"
#include "geometry.h"

class ODChartLabel;
class uiString;
class uiChart;
class uiChartSeries;


mExpClass(uiCharts) uiChartLabel : public CallBacker
{
public:
			uiChartLabel(uiChart*,
				     const uiString& uistr=uiString::empty(),
				     uiChartSeries* series=nullptr);
			~uiChartLabel();

    void		show(bool);
    void		setText(const uiString&);
    void		setAnchor(Geom::PointF,
				  Alignment all=mAlignment(Left,Bottom));
    void		updateCB(CallBacker*);

protected:
    ODChartLabel*	odlabel_ = nullptr;
};
