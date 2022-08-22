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
#include "separstr.h"

class uiChart;
class uiLineSeries;
class ODChartFillx;
class QImage;


mExpClass(uiCharts) uiVertGradientImg : public CallBacker
{
public:
				uiVertGradientImg();
				~uiVertGradientImg();

    const mQtclass(QImage&)	qImage() const	{ return *qimg_; }
    mQtclass(QImage&)		qImage()	{ return *qimg_; }

    virtual void		updateImg(const StepInterval<float>&) = 0;

    virtual FileMultiString	toString() const = 0;
    virtual void		fromString(const FileMultiString&) = 0;

protected:
    mQtclass(QImage*)		qimg_;
};
