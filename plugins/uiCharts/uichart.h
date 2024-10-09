#pragma once
/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uichartsmod.h"

#include "bufstring.h"
#include "callback.h"
#include "color.h"
#include "commondefs.h"
#include "geometry.h"
#include "odcommonenums.h"

#if QT_VERSION < x060000
    using namespace QtCharts;
#endif

class ODChart;
class QChart;
class uiChartAxis;
class uiChartSeries;
class i_chartMsgHandler;

mExpClass(uiCharts) uiChart : public CallBacker
{
public:
				uiChart();
    virtual			~uiChart();

    void			addAxis(uiChartAxis*,OD::Edge);
    void			addSeries(uiChartSeries*);
    OD::Color			backgroundColor() const;
    void			displayLegend(bool);
    Geom::PointF		mapToPosition(const Geom::PointF&,
					      uiChartSeries* series=nullptr);
    Geom::PointF		mapToValue(const Geom::PointF&,
					   uiChartSeries* series=nullptr);
    Geom::RectI			margins() const;
    int				numAxes(OD::Orientation) const;
    Geom::RectF			plotArea() const;
    uiChartAxis*		getAxis(OD::Orientation,int);
    void			removeAllAxes(OD::Orientation,
					      uiChartSeries* series=nullptr);
    void			removeAllSeries();
    void			removeAxis(uiChartAxis*);
    void			removeSeries(uiChartSeries*);
    void			setAcceptHoverEvents(bool);
    void			setBackgroundColor(const OD::Color&);
    void			setMargins(int l,int t,int r,int b);
    void			setPlotArea(const Geom::RectF&);
    void			setTitle(const char*);
    void			setTitle(const uiString&);
    BufferString		title() const;

    QChart*			getQChart();

    Notifier<uiChart>		plotAreaChanged;
    Notifier<uiChart>		needsRedraw;

protected:
    ODChart*			odchart_;
    ObjectSet<uiChartAxis>	axes_;

    void			axisRangeChgCB(CallBacker*);

private:
    i_chartMsgHandler*		msghandler_;
};
