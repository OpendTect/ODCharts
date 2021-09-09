#pragma once
/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		September 2021
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
