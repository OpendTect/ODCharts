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

class uiChartAxis;
class uiValueAxis;
class BufferString;
class MultiID;
namespace OD { class LineStyle; }
namespace Well { class Log; }

mClass(uiWellCharts) uiLogChart : public uiChart
{
public:
    enum Scale		{ Linear, Log10 };
    enum ZType		{ MD, TVD, TVDSS, TWT };

			uiLogChart(ZType ztype=MD,Scale scale=Linear);
			~uiLogChart();

    void		addLogCurve(const MultiID&,const char* lognm);
    void		addLogCurve(const MultiID&,const char* lognm,
				    const OD::LineStyle&);
    void		addLogCurve(const Well::Log&,const OD::LineStyle&,
				    float minval,float maxval,bool reverse);
    void		removeLogCurve(const MultiID&,const char* lognm);
    void		removeAllCurves();
    uiValueAxis*	getZAxis() const;
    void		setZRange(float minz,float maxz);
    void		setZRange(const Interval<float>&);

protected:
    ZType		ztype_;
    Scale		scale_;
    uiValueAxis*	zaxis_;

    void		makeZaxis();
    uiChartAxis*	makeLogAxis(const BufferString&,float,float,bool);
};
