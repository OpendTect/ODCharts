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
    class QLineSeries;
    class QScatterSeries;
    class QXYSeries;
}


mExpClass(uiCharts) uiChartSeries : public CallBacker
{
public:
    virtual		~uiChartSeries();

    void		attachAxis(uiChartAxis*);
    BufferString	name() const;
    void		setName(const char*);
    virtual void	initCallBacks() = 0;

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

    void		copyPoints(const uiLineSeries&);

    QtCharts::QLineSeries*	getQLineSeries();

protected:
    QtCharts::QLineSeries*	qlineseries_;
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
