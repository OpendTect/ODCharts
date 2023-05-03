#pragma once
/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uichartsmod.h"

#include "callback.h"
#include "color.h"
#include "enums.h"

class uiChart;
class uiLineSeries;
class uiVertGradientImg;
class ODChartFillx;


mExpClass(uiCharts) uiChartFillx : public CallBacker
{
public:
    enum FillDir	{ Left, Right };
    mDeclareEnumUtils(FillDir)
    enum FillType	{ NoFill, ColorFill, GradientFill };
    mDeclareEnumUtils(FillType)

			uiChartFillx();
			~uiChartFillx();

    void		addTo(uiChart*,const ObjectSet<uiLineSeries>&,
				FillDir fdir=Left);
    void		addTo(uiChart*,const ObjectSet<uiLineSeries>&,OD::Color,
				FillDir fdir=Left);
    void		setFillDir(FillDir,bool update=true);
    FillDir		fillDir() const;

    void		setFillType(FillType,bool update=true);
    FillType		fillType() const;

    void		setColor(OD::Color,bool update=true);
    OD::Color		color() const;

    void		setGradientImg(uiVertGradientImg*,bool update=true);
    uiVertGradientImg*	gradientImg() const;

    void		setBaseLine(float,bool update=true);
    void		setBaseLine(const ObjectSet<uiLineSeries>&,
				    bool update=true);
    void		setBaseLineSeriesEmpty(bool update=true);
    float		baseLineValue() const;
    bool		hasBaseLineSeries() const;

    void		updateCB(CallBacker*);

protected:
    ODChartFillx*	odfillx_;

};
