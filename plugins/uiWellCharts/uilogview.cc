/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		November 2020
________________________________________________________________________

-*/

#include "uilogview.h"

#include "uichart.h"
#include "uilogchart.h"
#include "uilogviewpropdlg.h"
#include "uimsg.h"
#include "uistrings.h"
#include "welldata.h"
#include "welllog.h"
#include "welllogset.h"

uiLogView::uiLogView( uiParent* p, const char* nm )
    : uiChartView(p,nm)
{
    setZoomStyle( VerticalZoom );
    mAttachCB(doubleClick, uiLogView::showSettingsCB);
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
    if ( logChart()->logcurves().size()==0 )
	return;
    if ( !propdlg_ )
	propdlg_ = new uiLogViewPropDlg( parent(), *logChart() );
    propdlg_->show();
}
