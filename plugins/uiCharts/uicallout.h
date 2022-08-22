#pragma once
/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uichartsmod.h"

#include "commondefs.h"
#include "geometry.h"
#include "color.h"


class ODCallout;
class uiString;
class uiChartSeries;

mExpClass(uiCharts) uiCallout
{
public:
			uiCallout(uiChartSeries*);
			~uiCallout();

    void		hide();
    void		setText(const uiString&);
    void		setAnchor(Geom::PointF,uiChartSeries* series=nullptr);
    void		setLine(const OD::Color&,int width);
    void		setBackground(const OD::Color&);
    void		setZValue(int zval);
    void		show();
    void		update();

protected:
    ODCallout*		odcallout_ = nullptr;
};
