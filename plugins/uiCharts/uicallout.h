#pragma once
/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		May 2021
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
