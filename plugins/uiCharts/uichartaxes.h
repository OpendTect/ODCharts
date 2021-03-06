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
class QPen;
class i_valueAxisMsgHandler;

mGlobal(uiCharts) void			toQPen(QPen&,const OD::LineStyle&,
					       bool usetransp=false,
					       bool cosmetic=true);
mGlobal(uiCharts) OD::LineStyle		fromQPen(const QPen&);


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
    OD::LineStyle	gridStyle() const;
    OD::LineStyle	minorGridStyle() const;

    OD::Orientation	orientation() const;

    void		setRange(float min,float max);
    void		setReverse(bool yn);
    void		setMinorTickCount(int);

    AxisType		type() const;

    QtCharts::QAbstractAxis*	getQAxis();

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
    void		setTickCount(int);

    void		setAxisLimits(const Interval<float>&,bool include=true);
    void		setAxisLimits(float min,float max,bool include=true);
    void		snapRange(float min,float max);

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

