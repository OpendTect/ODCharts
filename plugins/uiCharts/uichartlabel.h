#pragma once
/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		August 2021
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
