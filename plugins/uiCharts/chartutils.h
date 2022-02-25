#pragma once
/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		July 2021
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
