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
#include "draw.h"

namespace Well { class Log; }

class uiLogChart;

mExpClass(uiWellCharts) LogCurve
{
public:
			LogCurve();
			LogCurve(const MultiID&,const char*);
			~LogCurve();

    void		addTo(uiLogChart&);
    void		addTo(uiLogChart&,const IOPar&);
    void		addTo(uiLogChart&,const OD::LineStyle&);
    void		addTo(uiLogChart&,const OD::LineStyle&,float,
			      float,bool);
    void		removeFrom(uiLogChart&);

    void		setLineStyle(const OD::LineStyle&,bool usetransp=false);
    OD::LineStyle	lineStyle() const;
    void		setDisplayRange(float, float);
    void		setDisplayRange(const Interval<float>&);

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

protected:
    void			addLog(const Well::Log&);
    bool			initLog();

    MultiID			wellid_;
    BufferString 		wellname_;
    BufferString		logname_;
    BufferString		uomlbl_;
    BufferString		mnemlbl_;
    Interval<float>		dahrange_;
    Interval<float>		valrange_;
    Interval<float>		disprange_;
    OD::LineStyle		linestyle_;
    ObjectSet<uiLineSeries>	series_;
    uiChartAxis*		axis_ = nullptr;
};
