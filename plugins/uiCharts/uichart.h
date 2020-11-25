#pragma once

/*+
 * (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 * AUTHOR   : Nanne Hemstra
 * DATE     : November 2020
-*/

#include "uichartsmod.h"
#include "commondefs.h"

class ODChart;
namespace QtCharts { class QChart; }
class uiChartSeries;

mExpClass(uiCharts) uiChart
{
public:
			uiChart();
			~uiChart();

    void		addSeries(uiChartSeries&);

    QtCharts::QChart*	getQChart()	{ return (QtCharts::QChart*)odchart_; }

protected:
    ODChart*		odchart_;
};
