#pragma once

/*+
 * (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 * AUTHOR   : Nanne Hemstra
 * DATE     : November 2020
-*/

#include "uichartsmod.h"

class ODChart;
namespace QtCharts { class QChart; }

mExpClass(uiCharts) uiChart
{
public:
			uiChart();
			~uiChart();

    QtCharts::QChart*	getQChart()	{ return (QtCharts::QChart*)odchart_; }

protected:
    ODChart*		odchart_;
};
