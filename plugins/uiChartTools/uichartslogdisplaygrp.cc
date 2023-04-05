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
{
    logdisp_ = new uiLogView( this, "Log view" );

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
	disprange_ = lc->logcurves().first()->dispRange();

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
	if ( !specstyles_.isEmpty() && specstyles_.isPresent(log.name()) )
	{
	    const int sidx = specstyles_.indexOf( log.name() );
	    ls.fromString( specstyles_.get(sidx).second() );
	}
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


void uiChartsLogDisplayGrp::setDisplayProps(
		    const Well::DisplayProperties::Log& disp, bool forname )
{
    if ( !logdatas_.validIdx(wellidx_) )
	return;

    uiLogChart* lc = logdisp_->logChart();
    if ( !lc )
	return;

    OD::LineStyle ls( disp.getLineStyle() );
    for ( auto* logcurve : lc->logcurves() )
    {
	if ( !forname )
	{
	    logcurve->setLineStyle( ls );
	    normstyle_ = ls;
	}
	else if ( logcurve->logName()==disp.name_ )
	{
	    logcurve->setLineStyle( ls );
	    if ( !specstyles_.isPresent(disp.name_) )
	    {
		BufferString lsstr;
		ls.toString( lsstr );
		specstyles_.add( disp.name_, lsstr );
	    }
	    else
	    {
		const int idx = specstyles_.indexOf( disp.name_ );
		ls.toString( specstyles_.get(idx).second() );
	    }
	    break;
	}
    }
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
