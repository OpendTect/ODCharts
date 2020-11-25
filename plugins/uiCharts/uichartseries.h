#pragma once

/*+
 * (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 * AUTHOR   : Nanne Hemstra
 * DATE     : November 2020
-*/

#include "uichartsmod.h"
#include "commondefs.h"

namespace QtCharts
{
    class QAbstractSeries;
    class QLineSeries;
    class QXYSeries;
}


mExpClass(uiCharts) uiChartSeries
{
public:
			~uiChartSeries();

    QtCharts::QAbstractSeries*	getSeries()	{ return qabstractseries_; }

protected:
			uiChartSeries(QtCharts::QAbstractSeries*);

    QtCharts::QAbstractSeries*	qabstractseries_;
};


mExpClass(uiCharts) uiXYSeries : public uiChartSeries
{
public:
			~uiXYSeries();

    void		add(float x,float y);

protected:
			uiXYSeries(QtCharts::QXYSeries*);

    QtCharts::QXYSeries*	qxyseries_;
};


mExpClass(uiCharts) uiLineSeries : public uiXYSeries
{
public:
			uiLineSeries();
			~uiLineSeries();

protected:
    QtCharts::QLineSeries*	qlineseries_;
};
