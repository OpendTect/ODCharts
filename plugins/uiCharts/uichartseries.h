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
#include "chartutils.h"
#include "color.h"
#include "enums.h"
#include "geometry.h"
#include "uistring.h"

class uiCallout;
class uiChartAxis;
class i_xySeriesMsgHandler;

namespace OD		{ class LineStyle; }
namespace QtCharts
{
    class QAbstractSeries;
    class QAreaSeries;
    class QLineSeries;
    class QScatterSeries;
    class QXYSeries;
}


mExpClass(uiCharts) uiChartSeries : public CallBacker
{
public:
    virtual			~uiChartSeries();

    void			attachAxis(uiChartAxis*);
    BufferString		name() const;
    void			setName(const char*);

    bool			isVisible() const;
    void			setVisible(bool);

    virtual void		initCallBacks() = 0;

    QtCharts::QAbstractSeries*	getQSeries();

protected:
				uiChartSeries(QtCharts::QAbstractSeries*);

    QtCharts::QAbstractSeries*	qabstractseries_;
};


mExpClass(uiCharts) uiXYChartSeries : public uiChartSeries
{ mODTextTranslationClass(uiXYChartSeries)
public:
    virtual		~uiXYChartSeries();

    void		clear();
    void		append(float x,float y);
    void		append(int sz,float* xarr,float* yarr);
    float		x(int idx) const;
    float		y(int idx) const;
    void		replace(int sz,const float* xarr,const float* yarr,
				float xshift=0.f,float yshift=0.f);
    bool		replace_x(int sz,const float* arr,float xshift=0.f);
    bool		replace_y(int sz,const float* arr,float yshift=0.f);
    void		setAll_Y(float);
    int			size() const;
    bool		isEmpty() const;
    bool		validIdx(int) const;
    void		setCalloutTxt(const char*,int nrdecx=2,int nrdecy=2);

    void		setPointLabelsVisible(bool yn);
    void		setPointLabelsFormat(const char* fmt);
    virtual void	initCallBacks() override;

    CNotifier<uiXYChartSeries,const Geom::PointF&>	pressed;
    CNotifier<uiXYChartSeries,const Geom::PointF&>	released;
    CNotifier<uiXYChartSeries,const Geom::PointF&>	clicked;
    CNotifier<uiXYChartSeries,const Geom::PointF&>	doubleClicked;
    CNotifier<uiXYChartSeries,const Geom::PointF&>	hoverOn;
    CNotifier<uiXYChartSeries,const Geom::PointF&>	hoverOff;

protected:
			uiXYChartSeries(QtCharts::QXYSeries*);

    void		showCallout(CallBacker*);
    void		hideCallout(CallBacker*);

    QtCharts::QXYSeries*	qxyseries_ = nullptr;
    uiCallout*			callout_ = nullptr;
    BufferString		callouttxt_;
    int				nrdecx_ = 2;
    int				nrdecy_ = 2;

private:
    i_xySeriesMsgHandler*	msghandler_ = nullptr;
};


mExpClass(uiCharts) uiLineSeries : public uiXYChartSeries
{
public:
			uiLineSeries();
			~uiLineSeries();

    void		setLineStyle(const OD::LineStyle&,bool usetransp=false);
    OD::LineStyle	lineStyle() const;

    bool		pointsVisible() const;
    void		setPointsVisible(bool);

    bool		lineVisible() const;
    void		setLineVisible(bool);

    void		copyPoints(const uiLineSeries&);

    QtCharts::QLineSeries*	getQLineSeries();

protected:
    QtCharts::QLineSeries*	qlineseries_;
    OD::LineStyle::Type		savedlinetype_ = OD::LineStyle::Solid;
};


mExpClass(uiCharts) uiScatterSeries : public uiXYChartSeries
{
public:
    enum MarkerShape	{ Circle, Square };
    mDeclareEnumUtils(MarkerShape)

			uiScatterSeries();
			~uiScatterSeries();

    MarkerShape		shape() const;
    OD::Color		color() const;
    OD::Color		borderColor() const;
    float		markerSize() const;

    void		setShape(MarkerShape);
    void		setColor(OD::Color);
    void		setBorderColor(OD::Color);
    void		setMarkerSize(float);

    void		copyPoints(const uiScatterSeries&);

    QtCharts::QScatterSeries*	getQScatterSeries();

protected:
    QtCharts::QScatterSeries*	qscatterseries_;
};


mExpClass(uiCharts) uiAreaSeries : public uiChartSeries
{
public:
			uiAreaSeries(uiLineSeries*, uiLineSeries* lwr=nullptr);
			~uiAreaSeries();

    void		setBorderStyle(const OD::LineStyle&,
				       bool usetransp=false);
    OD::LineStyle	borderStyle() const;

    OD::Color		color() const;
    void		setColor(OD::Color);

    uiLineSeries*	upperSeries() const;
    uiLineSeries*	lowerSeries() const;
    void		setUpperSeries(uiLineSeries*);
    void		setLowerSeries(uiLineSeries*);

    bool		pointsVisible() const;
    void		setPointsVisible(bool);

    bool		linesVisible() const;
    void		setLinesVisible(bool);

    void		initCallBacks();

    QtCharts::QAreaSeries*	getQAreaSeries();

protected:
    QtCharts::QAreaSeries*	qareaseries_;
    uiLineSeries*		upperseries_ = nullptr;
    uiLineSeries*		lowerseries_ = nullptr;
    OD::LineStyle::Type		savedlinetype_ = OD::LineStyle::Solid;
};
