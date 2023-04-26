/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uichartslogdisplaygrp.h"

#include "draw.h"
#include "logcurve.h"
#include "uibuttongroup.h"
#include "uigeninput.h"
#include "uilogchart.h"
#include "uilogview.h"
#include "uilogviewpropdlg.h"
#include "uisellinest.h"
#include "uitabstack.h"
#include "uitoolbutton.h"

#include "welldisp.h"
#include "welllog.h"
#include "welllogset.h"
#include "wellselection.h"


uiChartsLogDisplayGrp::uiChartsLogDisplayGrp( uiParent* p )
    : uiLogDisplayGrp(p,"Log Display Group")
    , normstyle_(OD::LineStyle::Solid, 1, OD::Color::stdDrawColor(0))
    , specstyle_(OD::LineStyle::Solid, 1, OD::Color::stdDrawColor(1))
{
    logdisp_ = new uiLogView( this, "Log view" );
    logdisp_->setPrefWidth( 150 );
    logdisp_->setPrefHeight( 450 );

    auto* chart = new uiLogChart;
    chart->setZRange( 0.f, 1000.f );
    logdisp_->setChart( chart );
    chart->displayLegend( false );
    chart->setMargins( 0, 0, 0, 0 );
    logdisp_->setStretch( 2, 2 );

    auto* butgrp = new uiButtonGroup( this, "change log", OD::Horizontal );
    CallBack cbm = mCB(this,uiChartsLogDisplayGrp,changeWellButPush);
    prevlog_ = new uiToolButton( butgrp, uiToolButton::LeftArrow,
				 tr("Previous log"), cbm );
    nextlog_ = new uiToolButton( butgrp, uiToolButton::RightArrow,
				 tr("Next log"), cbm );

    new uiToolButton( butgrp, "view_all", tr("View all"),
			mCB(logdisp_,uiLogView,zoomResetCB) );

    new uiToolButton( butgrp, "settings", uiStrings::sSettings(),
			mCB(this,uiChartsLogDisplayGrp,showSettingsCB));

    butgrp->attach( centeredBelow, logdisp_ );
}


uiChartsLogDisplayGrp::~uiChartsLogDisplayGrp()
{}


void uiChartsLogDisplayGrp::addLogSelection(
				const ObjectSet<Well::SubSelData>& data )
{
    logdatas_ = data;
}


void uiChartsLogDisplayGrp::update()
{
    uiLogChart* lc = logdisp_->logChart();
    if ( propdlg_ && !lc->logcurves().isEmpty() )
    {
	disprange_ = lc->logcurves().first()->dispRange();
	normstyle_ = lc->logcurves().first()->lineStyle();
	specstyle_ = lc->logcurves().last()->lineStyle();
    }

    closeAndNullPtr( propdlg_ );
    lc->removeAllCurves();

    if ( !logdatas_.validIdx(wellidx_) )
	return;

    const Well::SubSelData* logsel = logdatas_[wellidx_];
    const Well::LogSet& logs = logsel->logs();
    OD::LineStyle ls;
    for ( int idx=0; idx<logs.size(); idx++ )
    {
	const auto& log = logs.getLog( idx );
	if ( idx==logs.size()-1 )
	    ls = specstyle_;
	else
	    ls = normstyle_;

	auto* lcurve = new LogCurve( logsel->wellName(), log );
	if ( !disprange_.isUdf() )
	    lcurve->setDisplayRange( disprange_ );

	lc->addLogCurve( lcurve, ls, false, true );
    }

    lc->setTitle( toUiString(logsel->wellName()) );

    prevlog_->display( logdatas_.size()>1 );
    nextlog_->display( logdatas_.size()>1 );
}


void uiChartsLogDisplayGrp::changeWellButPush( CallBacker* cb )
{
    mDynamicCastGet(uiToolButton*,but,cb)
    wellidx_ += but==nextlog_ ? 1 : -1;
    wellidx_ = getLimited( wellidx_, 0, logdatas_.size()-1 );
    update();
    nextlog_->setSensitive( wellidx_<logdatas_.size()-1 );
    prevlog_->setSensitive( wellidx_>0 );
}


void uiChartsLogDisplayGrp::showSettingsCB( CallBacker* )
{
    if ( !propdlg_ )
	propdlg_ = new uiLogViewPropDlg( this, logdisp_->logChart(), true,
					    false, true );

    propdlg_->show();
}
