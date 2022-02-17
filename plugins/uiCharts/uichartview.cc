/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		November 2020
________________________________________________________________________

-*/

#include "uichartview.h"
#include "uichart.h"
#include "uiobjbodyimpl.h"
#include "chartutils.h"

#include <QApplication>
#include <QChart>
#include <QChartView>

using namespace QtCharts;

class ODChartView : public uiObjBodyImpl<uiChartView,QChartView>
{
public:

ODChartView( uiChartView& hndle, uiParent* p, const char* nm )
    : uiObjBodyImpl<uiChartView,QChartView>(hndle,p,nm)
{
    setInteractive( true );
    setRenderHint(QPainter::Antialiasing);
}


~ODChartView()
{
}
    Qt::MouseButton		lastmousebut_ = Qt::NoButton;
    Qt::KeyboardModifiers	lastkeymod_ = Qt::NoModifier;
protected:
    bool		panning_ = false;
    QPointF		lastmousepos_;

    void		mouseMoveEvent(QMouseEvent*) override;
    void		mouseReleaseEvent(QMouseEvent*) override;
    void		mousePressEvent(QMouseEvent*) override;
    void		mouseDoubleClickEvent(QMouseEvent*) override;
};


void ODChartView::mouseMoveEvent( QMouseEvent* ev )
{
    lastmousebut_ = ev->button();
    lastkeymod_ = ev->modifiers();
    handle_.mouseMove.trigger( Geom::PointF(ev->pos().x(), ev->pos().y()) );

    if ( panning_ )
    {
	auto dpos = ev->pos() - lastmousepos_;
	if ( rubberBand()==VerticalRubberBand )
	    chart()->scroll( 0, dpos.y() );
	else if ( rubberBand()==HorizontalRubberBand )
	    chart()->scroll( dpos.x(), 0 );
	else
	    chart()->scroll( dpos.x(), dpos.y() );

	lastmousepos_ = ev->pos();
	ev->accept();
    }

    QChartView::mouseMoveEvent( ev );
}


void ODChartView::mouseReleaseEvent( QMouseEvent* ev )
{
    lastmousebut_ = ev->button();
    lastkeymod_ = ev->modifiers();
    handle_.mouseRelease.trigger( Geom::PointF(ev->pos().x(), ev->pos().y()) );

    if ( ev->button() == Qt::MiddleButton )
    {
	panning_ = false;
	auto dpos = ev->pos() - lastmousepos_;
	if ( rubberBand() == VerticalRubberBand )
	    chart()->scroll( 0, dpos.y() );
	else if ( rubberBand() == HorizontalRubberBand )
	    chart()->scroll( dpos.x(), 0 );
	else
	    chart()->scroll( dpos.x(), dpos.y() );

	lastmousepos_ = ev->pos();
	ev->accept();
	QApplication::restoreOverrideCursor();
    }
    else
	QChartView::mouseReleaseEvent( ev );

}


void ODChartView::mousePressEvent( QMouseEvent* ev )
{
    lastmousebut_ = ev->button();
    lastkeymod_ = ev->modifiers();
    handle_.mousePress.trigger( Geom::PointF(ev->pos().x(), ev->pos().y()) );

    if ( ev->button() == Qt::MiddleButton )
    {
	if ( rubberBand() == VerticalRubberBand )
	    QApplication::setOverrideCursor( QCursor(Qt::SizeVerCursor) );
	else if ( rubberBand() == HorizontalRubberBand )
	    QApplication::setOverrideCursor( QCursor(Qt::SizeHorCursor) );
	else
	    QApplication::setOverrideCursor( QCursor(Qt::SizeAllCursor) );

	panning_ = true;
	lastmousepos_ = ev->pos();
	ev->accept();
    }
    else
	QChartView::mousePressEvent( ev );

}


void ODChartView::mouseDoubleClickEvent( QMouseEvent* ev )
{
    lastmousebut_ = ev->button();
    lastkeymod_ = ev->modifiers();
    handle_.doubleClick.trigger();
}


// uiChartView
uiChartView::uiChartView( uiParent* p, const char* nm )
    : uiObject(p,nm,mkbody(p,nm))
    , doubleClick(this)
    , mouseMove(this)
    , mousePress(this)
    , mouseRelease(this)
{
}


uiChartView::~uiChartView()
{
    delete odchartview_;
}


void uiChartView::setChart( uiChart* chart )
{
    uichart_ = chart;
    if ( uichart_ )
	odchartview_->setChart( uichart_->getQChart() );
}


ODChartView& uiChartView::mkbody( uiParent* p, const char* nm )
{
    odchartview_ = new ODChartView( *this, p, nm );
    return *odchartview_;
}


void uiChartView::setBackgroundColor( const OD::Color& col )
{
    if ( col==OD::Color::NoColor() )
	odchartview_->setBackgroundBrush( QBrush(Qt::NoBrush) );
    else
    {
	QColor qcol;
	toQColor( qcol, col );
	odchartview_->setBackgroundBrush( QBrush(qcol) );
    }
}


void uiChartView::setMinimumSize( int w, int h )
{
    odchartview_->setMinimumSize( w, h );
}


void uiChartView::setZoomStyle( ZoomStyle style )
{
    QChartView::RubberBand rb = QChartView::NoRubberBand;
    if ( style == VerticalZoom )
	rb = QChartView::VerticalRubberBand;
    if ( style == HorizontalZoom )
	rb = QChartView::HorizontalRubberBand;
    if ( style == RectangleZoom )
	rb = QChartView::RectangleRubberBand;

    odchartview_->setRubberBand( rb );
}


OD::ButtonState uiChartView::mouseButton() const
{
    if ( odchartview_->lastmousebut_ & OD::LeftButton ||
	 odchartview_->lastmousebut_ & OD::MidButton ||
	 odchartview_->lastmousebut_ & OD::RightButton )
	return (OD::ButtonState) odchartview_->lastmousebut_;

    return OD::NoButton;
}


OD::ButtonState uiChartView::keyModifier() const
{
    if ( odchartview_->lastkeymod_ == Qt::ShiftModifier )
	return OD::ShiftButton;
    if ( odchartview_->lastkeymod_ == Qt::ControlModifier )
	return OD::ControlButton;
    if ( odchartview_->lastkeymod_ == Qt::MetaModifier )
	return OD::MetaButton;
    if ( odchartview_->lastkeymod_ == Qt::AltModifier )
	return OD::AltButton;
    if ( odchartview_->lastkeymod_ == Qt::KeypadModifier )
	return OD::Keypad;

    return OD::NoButton;
}
