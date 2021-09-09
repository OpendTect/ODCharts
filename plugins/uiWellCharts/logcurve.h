#pragma once

/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		August 2020
________________________________________________________________________

-*/

#include "uiwellchartsmod.h"
#include "logdata.h"
#include "uichartseries.h"
#include "color.h"
#include "draw.h"

namespace Well { class Log; }

class LogGradient;
class uiLogChart;
class uiChartFillx;

mExpClass(uiWellCharts) LogCurve : public LogData
{
public:
			LogCurve();
			LogCurve(const MultiID&,const char*);
			~LogCurve();

    void		addTo(uiLogChart&);
    void		addTo(uiLogChart&,const IOPar&);
    void		addTo(uiLogChart&,const OD::LineStyle&);
    void		addTo(uiLogChart&,const OD::LineStyle&,float min,
			      float max,bool reverse);
    void		addCurveFillTo(uiLogChart&);
    void		removeFrom(uiLogChart&);

    void		setLineStyle(const OD::LineStyle&,bool usetransp=false);
    OD::LineStyle	lineStyle() const;
    uiChartFillx*	leftFill()		{ return leftfill_; }
    uiChartFillx*	rightFill()		{ return rightfill_; }
    void		setFillToLog(const char* lognm,bool left=true);
    const char*		fillToLog(bool left=true);

    uiChartAxis*	getAxis() const		{ return axis_; }
    uiChartAxis*	getAxis()		{ return axis_; }

    void		setDisplayRange(const Interval<float>& range) override;
    void		fillPar(IOPar&) const;
    void		usePar(const IOPar&);

    ObjectSet<uiLineSeries>&	getSeries()	{ return series_; }

protected:
    void			addLog(const Well::Log&);
    BufferString		getFillPar(bool left) const;
    void			setFillPar(const char* fillstr,bool left=true);

    OD::LineStyle		linestyle_;
    float			pointsize_	= 2.f;
    ObjectSet<uiLineSeries>	series_;
    uiScatterSeries*		scatseries_ 	= nullptr;
    uiChartAxis*		axis_ 		= nullptr;
    uiChartFillx*		leftfill_	= nullptr;
    uiChartFillx*		rightfill_ 	= nullptr;
    BufferString		lefttolog_;
    BufferString		righttolog_;
};
