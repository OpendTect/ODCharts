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
#include "uilogviewtable.h"
#include "uilogviewpropdlg.h"
#include "uimsg.h"


uiLogView::uiLogView( uiParent* p, const char* nm, bool applyall )
    : uiChartView(p,nm)
    , applyall_(applyall)
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


void uiLogView::setLogViewTable( uiLogViewTable* tbl )
{
    logviewtbl_ = tbl;
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
    {
	propdlg_ = new uiLogViewPropDlg( parent(), logChart(), applyall_ );
	if ( applyall_ )
	    mAttachCB(propdlg_->applyPushed, uiLogView::applySettingsCB);
    }

    propdlg_->show();
}


void uiLogView::applySettingsCB( CallBacker* )
{
    if ( !logviewtbl_ || !propdlg_ || !applyall_ )
	return;

    auto* curlogchart = logChart();
    uiUserShowWait uisw( parent(), tr("Working: 0%") );
    IOPar settings = propdlg_->getCurrentSettings();
    for ( int idx=0; idx<logviewtbl_->size(); idx++ )
    {
	auto* logchart = logviewtbl_->getLogChart( idx );
	if ( !logchart || logchart==curlogchart )
	    continue;
	logchart->usePar( settings, true );
	uisw.setMessage(
	    tr("Working: %1 %").arg(int((idx+1)/logviewtbl_->size()*100.f)) );
    }
}


void uiLogView::zoomResetCB( CallBacker* )
{
    const Interval<float> rg = logChart()->getZAxis()->getAxisLimits();
    logChart()->setZRange( rg );
}
