/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		May 2021
________________________________________________________________________

-*/

#include "uichartaxes.h"

#include <QLogValueAxis>
#include <QValueAxis>

using namespace QtCharts;


void toQPen( QPen& qpen, const OD::LineStyle& ls, bool usetransp, bool cosmetic)
{
    QColor color = QColor( QRgb(ls.color_.rgb()) );
    if ( usetransp )
	color.setAlpha( 255-ls.color_.t() );

    QBrush qbrush( color );
    qpen = QPen( qbrush, ls.width_, Qt::PenStyle(ls.type_) );
    qpen.setCosmetic( cosmetic );
}


OD::LineStyle fromQPen( const QPen& qpen )
{
    const QColor qcol = qpen.color();
    const Color color( qcol.red(), qcol.green(), qcol.blue(), 255-qcol.alpha());
    return OD::LineStyle( OD::LineStyle::Type(qpen.style()), qpen.width(),
			  color );
}


uiChartAxis::uiChartAxis( QAbstractAxis* axis )
    : qabstractaxis_(axis)
{
}


uiChartAxis::~uiChartAxis()
{
}


void uiChartAxis::setTitleText( const char* title )
{
    qabstractaxis_->setTitleText( title );
}


BufferString uiChartAxis::titleText() const
{
    return BufferString( qabstractaxis_->titleText() );
}


void uiChartAxis::setLineStyle( const OD::LineStyle& ls, bool usetransp )
{
    QPen qpen;
    toQPen( qpen, ls, usetransp, true );
    qabstractaxis_->setLinePen( qpen );
}


void uiChartAxis::setGridStyle( const OD::LineStyle& ls, bool usetransp )
{
    QPen qpen;
    toQPen( qpen, ls, usetransp, true );
    qabstractaxis_->setGridLinePen( qpen );
}


void uiChartAxis::setMinorGridStyle( const OD::LineStyle& ls, bool usetransp )
{
    QPen qpen;
    toQPen( qpen, ls, usetransp, true );
    qabstractaxis_->setMinorGridLinePen( qpen );
}


OD::LineStyle uiChartAxis::lineStyle() const
{
    return fromQPen( qabstractaxis_->linePen() );
}


OD::LineStyle uiChartAxis::gridStyle() const
{
    return fromQPen( qabstractaxis_->gridLinePen() );
}


OD::LineStyle uiChartAxis::minorGridStyle() const
{
    return fromQPen( qabstractaxis_->minorGridLinePen() );
}


void uiChartAxis::setRange( float min, float max )
{
    qabstractaxis_->setRange( min, max );
}


void uiChartAxis::setReverse( bool yn )
{
    qabstractaxis_->setReverse( yn );
}


void uiChartAxis::setMinorTickCount( int count )
{
    auto* qvalueaxis = dynamic_cast<QValueAxis*>(qabstractaxis_);
    auto* qlogvalueaxis = dynamic_cast<QLogValueAxis*>(qabstractaxis_);
    if ( qvalueaxis )
	qvalueaxis->setMinorTickCount( count );
    if ( qlogvalueaxis )
	qlogvalueaxis->setMinorTickCount( count );
}


uiChartAxis::AxisType uiChartAxis::type() const
{
    return AxisType(qabstractaxis_->type());
}


QAbstractAxis* uiChartAxis::getQAxis()
{
    return qabstractaxis_;
}


// uiValueAxis
uiValueAxis::uiValueAxis()
    : uiChartAxis(new QValueAxis)
{
    qvalueaxis_ = dynamic_cast<QValueAxis*>(qabstractaxis_);
}


uiValueAxis::~uiValueAxis()
{
}


void uiValueAxis::setLabelFormat( const char* format )
{
    qvalueaxis_->setLabelFormat( format );
}


BufferString uiValueAxis::labelFormat() const
{
    return BufferString( qvalueaxis_->labelFormat() );
}


void uiValueAxis::setTickType( TickType type )
{
    QValueAxis::TickType qtype = type==TicksDynamic ? QValueAxis::TicksDynamic
						    : QValueAxis::TicksFixed;
    qvalueaxis_->setTickType( qtype );
}


void uiValueAxis::setTickInterval( float intv )
{
    qvalueaxis_->setTickInterval( qreal(intv) );
}


void uiValueAxis::setTickCount( int count )
{
    qvalueaxis_->setTickCount( count );
}



// uiLogValueAxis
uiLogValueAxis::uiLogValueAxis()
    : uiChartAxis(*this)
{
}


uiLogValueAxis::~uiLogValueAxis()
{
}


void uiLogValueAxis::setBase( float baseval )
{
    qlogvalueaxis_->setBase( qreal(baseval) );
}


void uiLogValueAxis::setLabelFormat( const char* format )
{
    qlogvalueaxis_->setLabelFormat( format );
}


BufferString uiLogValueAxis::labelFormat() const
{
    return BufferString( qlogvalueaxis_->labelFormat() );
}
