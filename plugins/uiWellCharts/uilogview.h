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

class uiLogChart;

mExpClass(uiWellCharts) uiLogView : public uiChartView
{
public:
			uiLogView(uiParent*,const char* nm);
			~uiLogView();

    void		setLogChart(uiLogChart*);
    uiLogChart*		logChart();

};
