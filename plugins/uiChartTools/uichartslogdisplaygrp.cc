/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uichartslogdisplaygrp.h"

#include "logcurve.h"
#include "uigeninput.h"
#include "uilogchart.h"
#include "uilogview.h"
#include "uilogviewpropdlg.h"
#include "uisellinest.h"
#include "uitabstack.h"
#include "uitoolbutton.h"

#include "welldata.h"
#include "welllog.h"
#include "welllogset.h"


uiChartsLogDisplayGrp::uiChartsLogDisplayGrp( uiParent* p )
    : uiLogDisplayGrp(p,"Log Display Group")
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

    auto* butgrp = new uiGroup( this, "change log" );
    CallBack cbm = mCB(this,uiChartsLogDisplayGrp,changeWellButPush);
    prevlog_ = new uiToolButton( butgrp, uiToolButton::LeftArrow,
				    tr("Previous log"), cbm );
    prevlog_->setHSzPol( uiObject::Undef );
    nextlog_ = new uiToolButton( butgrp, uiToolButton::RightArrow,
				    tr("Next log"), cbm );
    nextlog_->attach( rightOf, prevlog_ );
    nextlog_->setHSzPol( uiObject::Undef );

    auto* zoomreset = new uiToolButton( butgrp, "view_all",
					tr("View All Z"),
					mCB(logdisp_,uiLogView,zoomResetCB) );
    if ( nextlog_ )
	zoomreset->attach( rightOf, nextlog_ );

    auto* settings = new uiToolButton( butgrp, "settings",
				uiStrings::sSettings(),
				mCB(this,uiChartsLogDisplayGrp,showSettingsCB) );
    settings->attach( rightOf, zoomreset );

    butgrp->attach( centeredBelow, logdisp_ );
}


uiChartsLogDisplayGrp::~uiChartsLogDisplayGrp()
{}


void uiChartsLogDisplayGrp::setLogs( const ObjectSet<LogSelection>& data )
{
    logdatas_ = data;
}


void uiChartsLogDisplayGrp::update()
{
    uiLogChart* lc = logdisp_->logChart();
    lc->removeAllCurves();

    if ( !logdatas_.validIdx(wellidx_) )
	return;

    const LogSelection* logsel = logdatas_[wellidx_];
    const Well::LogSet& logs = logsel->logs();
    ls_.setSize( logs.size(), OD::LineStyle() );
    for ( int idx=0; idx<logs.size(); idx++ )
    {
	auto* log = new LogCurve( logsel->wellID(), logs.getLog(idx).name() );
	lc->addLogCurve( log, ls_[idx], false, true );
	if ( !disprange_.isUdf() )
	    log->setDisplayRange( disprange_ );
    }

    ls_.last().color_ = OD::Color::stdDrawColor( 0 );
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
    {
	BufferStringSet lognames;
	Well::LogSet& logs = logdatas_[wellidx_]->logs();
	logs.getNames( lognames );
	propdlg_ = new uiLogDisplayPropDlg( this, logdisp_->logChart(),
					   disprange_, lognames, ls_ );
	mAttachCB( propdlg_->changed, uiChartsLogDisplayGrp::settingsChgCB );
    }

    propdlg_->show();
}


void uiChartsLogDisplayGrp::settingsChgCB( CallBacker* )
{
    if ( !propdlg_ )
	return;

    const auto range = propdlg_->getRange();
    if ( range!=disprange_ )
    {
	disprange_ = range;
	for ( auto* lc : logdisp_->logChart()->logcurves() )
	    lc->setDisplayRange( range );
    }

    BufferStringSet lognames;
    Well::LogSet& logs = logdatas_[wellidx_]->logs();
    logs.getNames( lognames );
    ls_.setSize( lognames.size(), OD::LineStyle() );
    for ( int idx=0; idx<lognames.size(); idx++ )
    {
	const auto inpls = propdlg_->getLineStyle( lognames.get(idx) );
	if ( ls_[idx]!=inpls )
	{
	    ls_[idx] = inpls;
	    for ( auto* lc : logdisp_->logChart()->logcurves() )
	    {
		if ( lognames.get(idx)==lc->logName() )
		{
		    lc->setLineStyle( inpls );
		    break;
		}
	    }
	}
    }
}


uiDisplayLogsGrp::uiDisplayLogsGrp( uiParent* p, const Interval<float>& range,
				const BufferStringSet& lognms,
				const TypeSet<OD::LineStyle>& ls )
    : uiGroup(p)
    , changed(this)
{
    FloatInpIntervalSpec fspec(range);
    fspec.setName("Left",0).setName("Right",1);
    rangefld_ = new uiGenInput( this, tr("Display range"), fspec );
    rangefld_->setValue( range );
    mAttachCB(rangefld_->valuechanged, uiDisplayLogsGrp::changedCB);

    uiObject* attachobj = rangefld_->attachObj();
    for ( int idx=0; idx<lognms.size(); idx++ )
    {
	auto* lsfld = new uiSelLineStyle( this, ls[idx],
					  toUiString(lognms.get(idx)) );
	lsfld->attach( alignedBelow, attachobj );
	lognms_.add( lognms.get(idx) );
	linestyles_ += lsfld;
	mAttachCB( lsfld->changed, uiDisplayLogsGrp::changedCB );
	attachobj = lsfld->attachObj();
    }
}


uiDisplayLogsGrp::~uiDisplayLogsGrp()
{
    detachAllNotifiers();
    deepErase( linestyles_ );
}


Interval<float> uiDisplayLogsGrp::getRange() const
{
    return rangefld_->getFInterval();
}


OD::LineStyle uiDisplayLogsGrp::getLineStyle( const char* lognm ) const
{
    for ( int idx=0; idx<lognms_.size(); idx++ )
    {
	if ( lognms_.get(idx)==lognm )
	    return linestyles_[idx]->getStyle();
    }

    return OD::LineStyle();
}


void uiDisplayLogsGrp::changedCB( CallBacker* )
{
    changed.trigger();
}


uiLogDisplayPropDlg::uiLogDisplayPropDlg( uiParent* p,
					uiLogChart* lc,
					const Interval<float>& range,
					const BufferStringSet& inplognms,
					const TypeSet<OD::LineStyle>& inpls )
    : uiDialog(p,Setup(tr("Display Properties Editor"),mNoDlgTitle,
		       mTODOHelpKey))
    , changed(this)
    , logchart_(lc)
{
    setCtrlStyle( CloseOnly );
    setModal( false );
    setShrinkAllowed( true );

    tabs_ = new uiTabStack( this, "Properties" );
    chartgrp_ = new uiLogChartGrp( tabs_->tabGroup(), logchart_ );
    logsgrp_ = new uiDisplayLogsGrp( tabs_->tabGroup(), range, inplognms, inpls );

    tabs_->addTab( chartgrp_, tr("Chart properties") );
    tabs_->addTab( logsgrp_, uiStrings::sLog(2) );

    mAttachCB(logsgrp_->changed, uiLogDisplayPropDlg::changedCB);
}

uiLogDisplayPropDlg::~uiLogDisplayPropDlg()
{
    detachAllNotifiers();
}


Interval<float> uiLogDisplayPropDlg::getRange() const
{
    return logsgrp_->getRange();
}


OD::LineStyle uiLogDisplayPropDlg::getLineStyle( const char* lognm ) const
{
    return logsgrp_->getLineStyle( lognm );
}


void uiLogDisplayPropDlg::changedCB( CallBacker* )
{
    changed.trigger();
}
