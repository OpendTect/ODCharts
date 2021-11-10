#pragma once

/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		September 2021
________________________________________________________________________

-*/

#include "uiwellchartsmod.h"
#include "multiid.h"
#include "ranges.h"

namespace Well { class Log; }


mExpClass(uiWellCharts) LogData
{
public:
			LogData();
			LogData(const MultiID&,const char*);
			LogData(const LogData&) = delete;
    virtual		~LogData();
    void		operator=(const LogData&) = delete;

    MultiID		wellID() const		{ return wellid_; }
    BufferString	wellName() const	{ return wellname_; }
    BufferString	logName() const		{ return logname_; }
    const Well::Log*	wellLog() const;
    Interval<float>	zRange() const		{ return dahrange_; }
    Interval<float>	logRange() const	{ return valrange_; }
    Interval<float>	dispRange() const	{ return disprange_; }

    void		setDisplayRange(float left,float right);
    virtual void	setDisplayRange(const Interval<float>& range);
    void		fillPar(IOPar&) const;
    void		usePar(const IOPar&);

protected:
    bool		initLog();

    MultiID		wellid_;
    BufferString 	wellname_;
    BufferString	logname_;
    BufferString	uomlbl_;
    BufferString	mnemlbl_;
    Interval<float>	dahrange_;
    Interval<float>	valrange_;
    Interval<float>	disprange_;
};
