#pragma once

/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		August 2020
________________________________________________________________________

-*/

#include "uiwellchartsmod.h"
#include "uichartview.h"

class LogSeries;
namespace Well { class Data; }

mExpClass(uiWellCharts) uiLogView : public uiChartView
{
public:
			uiLogView(uiParent*,const char* nm);
			~uiLogView();

    void		addLog(const Well::Data&,const char* lognm);
    void		removeLog(const char* lognm);

protected:

    ObjectSet<const Well::Data>	welldata_;
    ObjectSet<LogSeries>	logseries_;
};