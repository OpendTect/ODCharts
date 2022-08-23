/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uichartslogmergegrp.h"

uiLogMergeGrp::uiLogMergeGrp( uiParent* p )
    : uiGroup(p,"Log Merge Display Group")
{
    logdisp_ = new uiLogView( grp, "Log merge" );
    logdisp_->setPrefWidth( 150 );
    logdisp_->setPrefHeight( 450 );

    auto* chart = new uiLogChart;
    chart->setZRange( 0.f, 1000.f );
    logdisp_->setChart( chart );
    chart->displayLegend( false );
    chart->setMargins( 0, 0, 0, 0 );
    logdisp_->setStretch( 2, 2 );

    uiGroup* butgrp = new uiGroup( grp, "change log" );
    if ( wellids_.size() > 1 )
    {
	CallBack cbm = mCB(this,uiWellLogMerger,changeWellButPush);
	prevlog_ = new uiToolButton( butgrp, uiToolButton::LeftArrow,
					tr("Previous log"), cbm );
	prevlog_->setHSzPol( uiObject::Undef );
	nextlog_ = new uiToolButton( butgrp, uiToolButton::RightArrow,
					tr("Next log"), cbm );
	nextlog_->attach( rightOf, prevlog_ );
	nextlog_->setHSzPol( uiObject::Undef );
    }

    auto* zoomreset = new uiToolButton( butgrp, "view_all",
					tr("View All Z"),
					mCB(logdisp_, uiLogView,zoomResetCB) );
    if ( nextlog_ )
	zoomreset->attach( rightOf, nextlog_ );

    auto* settings = new uiToolButton( butgrp, "settings",
				       uiStrings::sSettings(),
				   mCB(this, uiWellLogMerger,showSettingsCB) );
    settings->attach( rightOf, zoomreset );

    butgrp->attach( centeredBelow, logdisp_ );
}


uiLogMergeGrp::~uiLogMergeGrp()
{}


void uiLogMergeGrp::update()
{
    ConstRefMan<Well::Data> wd = Well::MGR().get( wellids_[wellidx_],
						  Well::LoadReqs::AllNoLogs() );
    BufferStringSet chosenlognms;
    selectedlogsfld_->getItems( chosenlognms );
    inpls_.setSize( chosenlognms.size(), OD::LineStyle() );
    uiLogChart* lc = logdisp_->logChart();
    lc->removeAllCurves();

    for ( int idx=0; idx<chosenlognms.size(); idx++ )
    {
	LogCurve* log = new LogCurve( wellids_[wellidx_],
				      chosenlognms.get(idx) );
	lc->addLogCurve( log, inpls_[idx], false, true );
	if ( !disprange_.isUdf() )
	    log->setDisplayRange( disprange_ );
    }
    lc->setTitle( toUiString(wd->name()) );

    if ( outlogs_.isEmpty() || !outlogs_.validIdx(wellidx_) )
	return;

    LogCurve* outlog = new LogCurve( wd->name(), *outlogs_[wellidx_] );
    lc->addLogCurve( outlog, outls_, false, true );
    if ( disprange_.isUdf() )
	disprange_ = outlog->dispRange();
    else
	outlog->setDisplayRange( disprange_ );
}


void uiLogMergeGrp::showSettingsCB( CallBacker* )
{
}



void uiLogMergeGrp::showSettingsCB( CallBacker* )
{
    if ( !propdlg_ )
    {
	BufferStringSet inplognms;
	selectedlogsfld_->getItems( inplognms );
	propdlg_ = new uiLogMergerPropDlg( this, logdisp_->logChart(),
					   disprange_, getOutputLogName(),
					   outls_, inplognms, inpls_ );
	mAttachCB( propdlg_->changed, uiLogMergeGrp::settingsChgCB );
    }

    propdlg_->show();
}


void uiLogMergeGrp::settingsChgCB( CallBacker* )
{
    if ( propdlg_ )
    {
	const auto range = propdlg_->getRange();
	if ( range!=disprange_ )
	{
	    disprange_ = range;
	    for ( auto* lc : logdisp_->logChart()->logcurves() )
		lc->setDisplayRange( range );
	}

	const auto ls = propdlg_->getOutLineStyle();
	if ( ls!=outls_ )
	{
	    outls_ = ls;
	    BufferString outlognm = getOutputLogName();
	    for ( auto* lc : logdisp_->logChart()->logcurves() )
	    {
		if ( outlognm==lc->logName() )
		{
		    lc->setLineStyle( ls );
		    break;
		}
	    }
	}

	BufferStringSet inplognms;
	selectedlogsfld_->getItems( inplognms );
	inpls_.setSize( inplognms.size(), OD::LineStyle() );
	for ( int idx=0; idx<inplognms.size(); idx++ )
	{
	    const auto inpls = propdlg_->getLineStyle( inplognms.get(idx) );
	    if ( inpls_[idx]!=inpls )
	    {
		inpls_[idx] = inpls;
		for ( auto* lc : logdisp_->logChart()->logcurves() )
		{
		    if ( inplognms.get(idx)==lc->logName() )
		    {
			lc->setLineStyle( inpls );
			break;
		    }
		}
	    }
	}
    }
}
