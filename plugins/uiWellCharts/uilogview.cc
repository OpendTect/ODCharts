/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uilogview.h"

#include "uichart.h"
#include "uichartaxes.h"
#include "uilogchart.h"
#include "uilogviewpropdlg.h"


uiLogView::uiLogView( uiParent* p, const char* nm )
    : uiChartView(p,nm)
{
    setZoomStyle( VerticalZoom );
}


uiLogView::~uiLogView()
{
    detachAllNotifiers();
    delete propdlg_;
}


void uiLogView::setLogChart( uiLogChart* newchart )
{
    setChart( newchart );
}


uiLogChart* uiLogView::logChart()
{
    uiChart* ch = chart();
    return dynamic_cast<uiLogChart*>(ch);
}


void uiLogView::showSettingsCB( CallBacker* )
{
    if ( logChart()->logcurves().isEmpty() )
	return;

    if ( !propdlg_ )
	propdlg_ = new uiLogViewPropDlg( parent(), logChart() );

    propdlg_->show();
}


void uiLogView::zoomResetCB( CallBacker* )
{
    const Interval<float> rg = logChart()->getZAxis()->getAxisLimits();
    logChart()->setZRange( rg );
}
