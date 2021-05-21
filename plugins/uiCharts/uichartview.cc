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
}


~ODChartView()
{
}

};


uiChartView::uiChartView( uiParent* p, const char* nm )
    : uiObject(p,nm,mkbody(p,nm))
{
}


uiChartView::~uiChartView()
{
    odchartview_->setChart( nullptr );
    delete uichart_;
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
