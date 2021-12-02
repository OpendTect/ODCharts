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
class MarkerLine;
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
    mDeclareEnumUtils(Scale)
    enum ZType		{ MD, TVD, TVDSS, TVDSD, TWT };
    mDeclareEnumUtils(ZType)

			uiLogChart(ZType ztype=MD,Scale scale=Linear);
			~uiLogChart();

    bool		hasLogCurve(const MultiID&,const char* lognm);
    LogCurve*		getLogCurve(const MultiID&,const char* lognm);
    LogCurve*		getLogCurve(const char* lognm);
    void		addLogCurve(LogCurve*,const OD::LineStyle&,
				    bool show_wellnm=true,bool show_uom=true);
			// LogCurve becomes mine
    void		addLogCurve(const MultiID&,const char* lognm);
    void		addLogCurve(const MultiID&,const char* lognm,
				    const OD::LineStyle&);
    void		addLogCurve(const MultiID&,const char* lognm,
				    const OD::LineStyle&,float min,float max,
				    bool reverse,bool show_wellnm=true,
				    bool show_uom=true);
    void		removeLogCurve(const MultiID&,const char* lognm);
    void		removeLogCurve(const char* lognm);
    void		removeAllCurves();

    bool		hasMarker(const MultiID&,const char* markernm);
    void		addMarker(const MultiID&,const char* markernm);
    void		addMarker(const MultiID&,const char* markernm,
				  const OD::LineStyle&);
    void		removeMarker(const MultiID&,const char* markernm);
    void		removeAllMarkers();

    void		setZType(ZType);
    uiValueAxis*	getZAxis() const;
    Interval<float>	getActualZRange() const;
    void		setZRange(float minz,float maxz);
    void		setZRange(const Interval<float>&);
    void		updateZAxisTitle();
    uiChartAxis*	makeLogAxis(const BufferString&,float min,float max,
				    bool reverse);
    BufferStringSet	wellNames() const;
    TypeSet<MultiID>	wellIDs() const;
    BufferStringSet	getDispLogsForID(const MultiID&) const;
    BufferStringSet	getDispMarkersForID(const MultiID&) const;
    void		setScale(Scale);
    Scale		getScale() const	{ return scale_; }

    void		fillPar(IOPar&) const;
    void		usePar(const IOPar&);

    ObjectSet<LogCurve>&	logcurves()	{ return logcurves_; }
    ObjectSet<MarkerLine>&	markers()	{ return markers_; }

    Notifier<uiLogChart>	logChange;
    Notifier<uiLogChart>	markerChange;

protected:
    ZType			ztype_;
    Scale			scale_;
    uiValueAxis*		zaxis_;
    ObjectSet<LogCurve>		logcurves_;
    ObjectSet<MarkerLine>	markers_;

    void			makeZaxis();
};
