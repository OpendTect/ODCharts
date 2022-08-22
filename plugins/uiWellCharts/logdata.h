#pragma once
/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uiwellchartsmod.h"

#include "multiid.h"
#include "ranges.h"
#include "wellchartcommon.h"
#include "wellbase.h"

namespace Well { class Log; }

mExpClass(uiWellCharts) LogData : public WellData
{
public:
			LogData();
			LogData(const MultiID&,const char*);
			LogData(const LogData&) = delete;
    virtual		~LogData();
    void		operator=(const LogData&) = delete;

    BufferString	logName() const		{ return logname_; }
    Interval<float>	dahRange() const	{ return dahrange_; }
    Interval<float>	zRange() const		{ return zrange_; }
    Interval<float>	logRange() const	{ return valrange_; }
    Interval<float>	dispRange();

    void		setDisplayRange(float left,float right);
    virtual void	setDisplayRange(const Interval<float>& range);
    virtual void	setZType(uiWellCharts::ZType, bool) override;
    void		setLogName(const char*);
    void		fillPar(IOPar&) const;
    void		usePar(const IOPar&, bool styleonly=false);

protected:
    bool		initLog();
    bool		initLog(const Well::Log&);
    bool		initLog(const char*,const Well::Log&);
    void		copyFrom(const LogData&);
    ConstRefMan<Well::Data>	getWD() const override;

    BufferString	logname_;
    BufferString	uomlbl_;
    BufferString	mnemlbl_;
    Interval<float>	dahrange_;
    Interval<float>	zrange_;
    Interval<float>	valrange_;
    Interval<float>	disprange_;
};
