#pragma once
/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		November 2020
________________________________________________________________________

-*/

#include "uichartsmod.h"

#include "bufstring.h"

class uiChartAxis;
namespace OD		{ class LineStyle; }
namespace QtCharts
{
    class QAbstractSeries;
    class QLineSeries;
    class QScatterSeries;
    class QXYSeries;
}


mExpClass(uiCharts) uiChartSeries
{
public:
			~uiChartSeries();

    void		attachAxis(uiChartAxis*);
    BufferString	name() const;
    void		setName(const char*);

    QtCharts::QAbstractSeries*	getQSeries();

protected:
			uiChartSeries(QtCharts::QAbstractSeries*);

    QtCharts::QAbstractSeries*	qabstractseries_;
};


mExpClass(uiCharts) uiXYChartSeries : public uiChartSeries
{
public:
    virtual		~uiXYChartSeries();

    void		clear();
    void		add(float x,float y);
    int			size() const;
    bool		isEmpty() const;

protected:
			uiXYChartSeries(QtCharts::QXYSeries*);

    QtCharts::QXYSeries*	qxyseries_;
};


mExpClass(uiCharts) uiLineSeries : public uiXYChartSeries
{
public:
			uiLineSeries();
			~uiLineSeries();

    void		append(float, float);
    void		append(int, float*, float*);
    void		setLineStyle(const OD::LineStyle&,bool usetransp=false);

    OD::LineStyle	lineStyle() const;

protected:
    QtCharts::QLineSeries*	qlineseries_;
};


mExpClass(uiCharts) uiScatterSeries : public uiXYChartSeries
{
public:
			uiScatterSeries();
			~uiScatterSeries();

protected:
    QtCharts::QScatterSeries*	qscatterseries_;
};
