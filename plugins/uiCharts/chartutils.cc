/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		July 2021
________________________________________________________________________

-*/

#include "chartutils.h"

#include <QBrush>
#include <QColor>
#include <QPen>

void toQColor( QColor& qcol, const OD::Color& col, bool usetransp )
{
    if ( col==OD::Color::NoColor() )
	qcol = QColor();
    else
    {
	qcol = QColor( QRgb(col.rgb()) );
	if ( usetransp )
	    qcol.setAlpha( 255-col.t() );
    }
}


OD::Color fromQColor( const QColor& qcol )
{
    if ( !qcol.isValid() )
	return OD::Color::NoColor();

    return OD::Color( qcol.red(), qcol.green(), qcol.blue(), 255-qcol.alpha() );
}


void toQPen( QPen& qpen, const OD::LineStyle& ls, bool usetransp, bool cosmetic)
{
    QColor qcol;
    toQColor( qcol, ls.color_, usetransp );

    QBrush qbrush( qcol );
    qpen = QPen( qbrush, ls.width_, Qt::PenStyle(ls.type_) );
    qpen.setCosmetic( cosmetic );
}


OD::LineStyle fromQPen( const QPen& qpen )
{
    const OD::Color color = fromQColor( qpen.color() );
    return OD::LineStyle( OD::LineStyle::Type(qpen.style()), qpen.width(),
			  color );
}


OD::Edge fromSide( uiRect::Side side )
{
    if ( side==uiRect::Top )
	return OD::Edge::Top;
    if ( side==uiRect::Bottom )
	return OD::Edge::Bottom;
    if ( side==uiRect::Left )
	return OD::Edge::Left;

    return OD::Edge::Right;
}
