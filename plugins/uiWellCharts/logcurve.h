#pragma once

/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		August 2020
________________________________________________________________________

-*/

#include "uiwellchartsmod.h"
#include "uichartseries.h"
#include "multiid.h"
#include "color.h"
#include "draw.h"

namespace Well { class Log; }

class uiLogChart;
class uiChartFillx;

mExpClass(uiWellCharts) LogCurve
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
    void		setDisplayRange(float left,float right);
    void		setDisplayRange(const Interval<float>& range);
    uiChartFillx*	leftFill()		{ return leftfill_; }
    uiChartFillx*	rightFill()		{ return rightfill_; }
    void		setFillToLog(const char* lognm,bool left=true);


    MultiID		wellID() const		{ return wellid_; }
    BufferString	wellName() const	{ return wellname_; }
    BufferString	logName() const		{ return logname_; }
    const Well::Log*	wellLog() const;
    Interval<float>	zRange() const		{ return dahrange_; }
    Interval<float>	logRange() const	{ return valrange_; }
    Interval<float>	dispRange() const	{ return disprange_; }
    uiChartAxis*	getAxis() const		{ return axis_; }
    uiChartAxis*	getAxis()		{ return axis_; }

    void		fillPar(IOPar&) const;
    void		usePar(const IOPar&);

    ObjectSet<uiLineSeries>&	getSeries()	{ return series_; }

protected:
    void			addLog(const Well::Log&);
    bool			initLog();
    BufferString		getFillPar(bool left) const;
    void			setFillPar(const char* fillstr,bool left=true);

    MultiID			wellid_;
    BufferString 		wellname_;
    BufferString		logname_;
    BufferString		uomlbl_;
    BufferString		mnemlbl_;
    Interval<float>		dahrange_;
    Interval<float>		valrange_;
    Interval<float>		disprange_;
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
