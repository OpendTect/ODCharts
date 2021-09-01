#pragma once
/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		May 2021
________________________________________________________________________

-*/

#include "uichartsmod.h"

#include "bufstring.h"
#include "draw.h"
#include "oduicommon.h"

namespace QtCharts
{
    class QAbstractAxis;
    class QLogValueAxis;
    class QValueAxis;
}

class i_valueAxisMsgHandler;


mExpClass(uiCharts) uiChartAxis : public CallBacker
{
public:
    virtual		~uiChartAxis();

    enum AxisType	{ NoAxis, Value, BarCategory, Category, DateTime,
			  LogValue };

    void		setTitleText(const char*);
    BufferString	titleText() const;

    void		setLineStyle(const OD::LineStyle&,bool usetransp=false);
    void		setGridStyle(const OD::LineStyle&,bool usetransp=false);
    void		setMinorGridStyle(const OD::LineStyle&,
					  bool usetransp=false);

    OD::LineStyle	lineStyle() const;
    OD::LineStyle	getGridStyle() const;
    OD::LineStyle	getMinorGridStyle() const;

    bool		gridVisible() const;
    void		setGridLineVisible(bool);
    bool		minorGridVisible() const;
    void		setMinorGridLineVisible(bool);

    OD::Orientation	orientation() const;

    void		setRange(const Interval<float>&);
    void		setRange(float min,float max);
    Interval<float>	range() const;

    void		setReverse(bool yn);
    bool		reversed() const;

    void		setMinorTickCount(int);
    int			getMinorTickCount() const;

    void		setTickCount(int);
    int			getTickCount() const;

    void		setTickInterval(float);
    float		getTickInterval() const;

    AxisType		type() const;

    QtCharts::QAbstractAxis*	getQAxis();

    CNotifier<uiChartAxis,const Interval<float>&>	rangeChanged;

protected:
			uiChartAxis(QtCharts::QAbstractAxis*);

    QtCharts::QAbstractAxis*	qabstractaxis_;
};


mExpClass(uiCharts) uiValueAxis : public uiChartAxis
{
public:
			uiValueAxis();
			~uiValueAxis();

    enum TickType	{ TicksDynamic, TicksFixed };

    void 		setLabelFormat(const char*);
    BufferString	labelFormat() const;

    void 		setTickType(TickType);
    void		setTickInterval(float);

    void		setAxisLimits(const Interval<float>&,bool include=true);
    void		setAxisLimits(float min,float max,bool include=true);
    void		snapRange(float min,float max);
    Interval<float>	getAxisLimits();

protected:
    QtCharts::QValueAxis*	qvalueaxis_;
    Interval<float>		axislimits_;

private:
    i_valueAxisMsgHandler*	msghandler_;
};


mExpClass(uiCharts) uiLogValueAxis : public uiChartAxis
{
public:
			uiLogValueAxis();
			~uiLogValueAxis();


    void 		setLabelFormat(const char*);
    BufferString	labelFormat() const;

    void		setBase(float);

protected:
    QtCharts::QLogValueAxis*	qlogvalueaxis_;

};

