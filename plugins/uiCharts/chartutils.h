#pragma once
/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uichartsmod.h"
#include "color.h"
#include "draw.h"
#include "odcommonenums.h"
#include "uigeom.h"

class QPen;
class QColor;

mGlobal(uiCharts) void			toQColor(QColor&,const OD::Color&,
						 bool usetransp=false);
mGlobal(uiCharts) OD::Color		fromQColor(const QColor&);

mGlobal(uiCharts) void			toQPen(QPen&,const OD::LineStyle&,
					       bool usetransp=false,
					       bool cosmetic=true);
mGlobal(uiCharts) OD::LineStyle		fromQPen(const QPen&);

mGlobal(uiCharts) OD::Edge		fromSide(uiRect::Side);
