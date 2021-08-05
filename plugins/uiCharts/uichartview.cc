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
    else if ( ev->button() == Qt::RightButton )
    {
	chart()->zoomReset();
	ev->accept();
    }
    else
	QChartView::mouseReleaseEvent( ev );
}


void ODChartView::mousePressEvent( QMouseEvent* ev )
{
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
    else if ( ev->button() == Qt::RightButton )
    {
	chart()->zoomReset();
	ev->accept();
    }
    else
	QChartView::mousePressEvent( ev );
}


void ODChartView::mouseDoubleClickEvent( QMouseEvent* ev )
{
    handle_.doubleClick.trigger();
}

// uiChartView
uiChartView::uiChartView( uiParent* p, const char* nm )
    : uiObject(p,nm,mkbody(p,nm))
    , doubleClick(this)
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


void uiChartView::zoomResetCB( CallBacker* )
{
    odchartview_->chart()->zoomReset();
}
