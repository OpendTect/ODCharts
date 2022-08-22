#pragma once
/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uiwellchartsmod.h"
#include "uichart.h"
#include "wellchartcommon.h"

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
    inline StringView ZMajorGrid()		{ return "Z Major Grid"; }
    inline StringView ZMajorGridStyle()	{ return "Z Major Gridstyle"; }
    inline StringView ZMinorGrid()		{ return "Z Minor Grid"; }
    inline StringView ZMinorGridStyle()	{ return "Z Minor Gridstyle"; }
    inline StringView LogMajorGrid()		{ return "Log Major Grid"; }
    inline StringView LogMajorGridStyle()	{ return "Log Major Gridstyle";}
    inline StringView LogMinorGrid()		{ return "Log Minor Grid"; }
    inline StringView LogMinorGridStyle()	{ return "Log Minor Gridstyle";}
};

mExpClass(uiWellCharts) uiLogChart : public uiChart
{
public:
			uiLogChart(uiWellCharts::ZType ztype=uiWellCharts::MD,
				uiWellCharts::Scale scale=uiWellCharts::Linear);
			~uiLogChart();

    bool		hasLogCurve(const MultiID&,const char* lognm);
    LogCurve*		getLogCurve(const MultiID&,const char* lognm);
    LogCurve*		getLogCurve(const char* lognm);
    void		addLogCurve(LogCurve*,
				    bool show_wellnm=true,bool show_uom=true);
			// LogCurve becomes mine
    void		addLogCurve(LogCurve*,const OD::LineStyle&,
				    bool show_wellnm=true,bool show_uom=true);
			// LogCurve becomes mine
    void		addLogCurves(ObjectSet<LogCurve>&,
				     bool show_wellnm=true,bool show_uom=true);
			// LogCurves become mine
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
    void		addMarker(const MultiID&,const char* markernm,
				  bool show_wellnm=true);
    void		addMarker(const MultiID&,const char* markernm,
				  const OD::LineStyle&,bool show_wellnm=true);
    void		removeMarker(const MultiID&,const char* markernm);
    void		removeAllMarkers();
    float		getMarkerZ(const char*) const;

    uiWellCharts::ZType	zType() const		{ return ztype_; }
    void		setZType(uiWellCharts::ZType,bool force=false);
    uiValueAxis*	getZAxis() const;
    Interval<float>	getActualZRange() const;
    void		setZRange(float minz,float maxz);
    void		setZRange(const Interval<float>&);
    void		updateZAxisTitle();
    uiChartAxis*	makeLogAxis(const BufferString&,float min,float max,
				    bool reverse);
    BufferStringSet	wellNames() const;
    DBKeySet		wellIDs() const;
    BufferStringSet	getDispLogsForID(const MultiID&) const;
    BufferStringSet	getDispMarkersForID(const MultiID&) const;
    void		setScale(uiWellCharts::Scale);
    uiWellCharts::Scale	getScale() const	{ return scale_; }

    void		setZShift(float);
    float		zShift() const		{ return zshift_; }

    void		fillPar(IOPar&) const;
    void		usePar(const IOPar&,bool styleonly=false);

    ObjectSet<LogCurve>&	logcurves()	{ return logcurves_; }
    ObjectSet<MarkerLine>&	markers()	{ return markers_; }

    Notifier<uiLogChart>	logChange;
    Notifier<uiLogChart>	markerChange;

protected:
    uiWellCharts::ZType		ztype_;
    uiWellCharts::Scale		scale_;
    uiValueAxis*		zaxis_;
    ObjectSet<LogCurve>		logcurves_;
    ObjectSet<MarkerLine>	markers_;
    float			zshift_ = 0.f;

    void			makeZaxis();
};
