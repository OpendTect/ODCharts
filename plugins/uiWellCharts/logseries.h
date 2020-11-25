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

namespace Well
{
    class Log;
}

mExpClass(uiWellCharts) LogSeries : public uiLineSeries
{
public:
			LogSeries();
			~LogSeries();

    void		setWellLog(const Well::Log&);

protected:
    void		updateSeries();

    const Well::Log*	log_	= nullptr;
};
