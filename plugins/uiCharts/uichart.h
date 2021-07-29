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
#include "callback.h"
#include "color.h"
#include "commondefs.h"
#include "geometry.h"
#include "oduicommon.h"

class ODChart;
namespace QtCharts { class QChart; }
class uiChartAxis;
class uiChartSeries;
class i_chartMsgHandler;

mExpClass(uiCharts) uiChart : public CallBacker
{
public:
			uiChart();
    virtual		~uiChart();

    void		addAxis(uiChartAxis*,OD::Edge);
    void		addSeries(uiChartSeries*);
    OD::Color		backgroundColor() const;
    void		displayLegend(bool);
    Geom::RectI		margins() const;
    Geom::PointF	mapToPosition(const Geom::PointF&,
				      uiChartSeries* series=nullptr);
    Geom::PointF	mapToValue(const Geom::PointF&,
				   uiChartSeries* series=nullptr);
    int			numAxes(OD::Orientation) const;
    Geom::RectF		plotArea() const;
    void		removeAllAxes(OD::Orientation,
				      uiChartSeries* series=nullptr);
    void		removeAllSeries();
    void		removeAxis(uiChartAxis*);
    void		removeSeries(uiChartSeries*);
    void		setAcceptHoverEvents(bool);
    void		setBackgroundColor(const OD::Color&);
    void		setMargins(int, int, int, int);
    void		setPlotArea(const Geom::RectF&);
    void		setTitle(const char*);

    BufferString	title() const;

    QtCharts::QChart*	getQChart()	{ return (QtCharts::QChart*)odchart_; }

    Notifier<uiChart>	plotAreaChanged;

protected:
    ODChart*		odchart_;

private:
    i_chartMsgHandler*	msghandler_;
};
