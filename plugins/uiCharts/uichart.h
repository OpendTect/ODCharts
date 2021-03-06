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
#include "commondefs.h"
#include "geometry.h"
#include "oduicommon.h"

class ODChart;
namespace QtCharts { class QChart; }
class uiChartAxis;
class uiChartSeries;

mExpClass(uiCharts) uiChart : public CallBacker
{
public:
			uiChart();
    virtual		~uiChart();

    void		addAxis(uiChartAxis*,OD::Edge);
    void		addSeries(uiChartSeries*);
    void		displayLegend(bool);
    Geom::PointF	mapToPosition(const Geom::PointF&,
				      uiChartSeries* series=nullptr);
    Geom::PointF	mapToValue(const Geom::PointF&,
				   uiChartSeries* series=nullptr);
    void		removeAllAxes(OD::Orientation,
				      uiChartSeries* series=nullptr);
    void		removeAllSeries();
    void		removeAxis(uiChartAxis*);
    void		removeSeries(uiChartSeries*);
    void		setAcceptHoverEvents(bool);
    void		setTitle(const char*);

    BufferString	title() const;

    QtCharts::QChart*	getQChart()	{ return (QtCharts::QChart*)odchart_; }

protected:
    ODChart*		odchart_;
};
