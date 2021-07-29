#pragma once
/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		May 2021
________________________________________________________________________

-*/

#include "uiwellchartsmod.h"
#include "uichart.h"
#include "enums.h"

class uiChartAxis;
class uiValueAxis;
class BufferString;
class LogCurve;
class MultiID;
namespace OD { class LineStyle; }
namespace Well { class Log; }

namespace sKey
{
    inline FixedString ZMajorGrid()		{ return "Z Major Grid"; }
    inline FixedString ZMajorGridStyle()	{ return "Z Major Gridstyle"; }
    inline FixedString ZMinorGrid()		{ return "Z Minor Grid"; }
    inline FixedString ZMinorGridStyle()	{ return "Z Minor Gridstyle"; }
    inline FixedString LogMajorGrid()		{ return "Log Major Grid"; }
    inline FixedString LogMajorGridStyle()	{ return "Log Major Gridstyle";}
    inline FixedString LogMinorGrid()		{ return "Log Minor Grid"; }
    inline FixedString LogMinorGridStyle()	{ return "Log Minor Gridstyle";}
};

mClass(uiWellCharts) uiLogChart : public uiChart
{
public:
    enum Scale		{ Linear, Log10 };
    mDeclareEnumUtils(Scale);
    enum ZType		{ MD, TVD, TVDSS, TWT };
    mDeclareEnumUtils(ZType);

			uiLogChart(ZType ztype=MD,Scale scale=Linear);
			~uiLogChart();

    bool		hasLogCurve(const MultiID&, const char*);
    void		addLogCurve(const MultiID&,const char*);
    void		addLogCurve(const MultiID&,const char*,
				    const OD::LineStyle&);
    void		addLogCurve(const MultiID&,const char*,
				    const OD::LineStyle&, float, float, bool);
    void		removeAllCurves();
    uiValueAxis*	getZAxis() const;
    Interval<float>	getActualZRange() const;
    void		setZRange(float minz,float maxz);
    void		setZRange(const Interval<float>&);
    uiChartAxis*	makeLogAxis(const BufferString&,float,float,bool);
    BufferStringSet	wellNames() const;

    void		fillPar(IOPar&) const;
    void		usePar(const IOPar&);

    ObjectSet<LogCurve>&	logcurves()	{ return logcurves_; }

    Notifier<uiLogChart>	logChange;


protected:
    ZType		ztype_;
    Scale		scale_;
    uiValueAxis*	zaxis_;
    ObjectSet<LogCurve>	logcurves_;

    void		makeZaxis();

};
