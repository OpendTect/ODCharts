/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		July 2021
________________________________________________________________________

-*/


#include "uilogviewpropdlg.h"

#include "draw.h"
#include "logcurve.h"
#include "uichartaxes.h"
#include "uicolor.h"
#include "uigeninput.h"
#include "uilistbox.h"
#include "uilogchart.h"
#include "uilogcurveprops.h"
#include "uimain.h"
#include "uisellinest.h"
#include "wellchartcommon.h"


// uiLogViewPropDlg
uiLogViewPropDlg::uiLogViewPropDlg( uiParent* p, uiLogChart& logchart )
    : uiDialog(p,Setup(tr("Display Properties Editor"),mNoDlgTitle,
		       mTODOHelpKey))
    , logchart_(logchart)
{
    setCtrlStyle( CloseOnly );
    setModal( false );
    setShrinkAllowed( true );

    chartgrp_ = new uiLogChartGrp( this, logchart_ );

    logsgrp_ = new uiLogsGrp( this, logchart_ );
    logsgrp_->attach( alignedBelow, chartgrp_ );
    mAttachCB( logchart_.logChange, uiLogViewPropDlg::updateCB );
    mAttachCB( uiMain::theMain().topLevel()->windowClosed,
	       uiLogViewPropDlg::closeCB );
}


uiLogViewPropDlg::~uiLogViewPropDlg()
{
    detachAllNotifiers();
}


void uiLogViewPropDlg::updateCB( CallBacker* )
{
    logsgrp_->update();
}


void uiLogViewPropDlg::closeCB( CallBacker* )
{
    close();
}


// uiLogChartGrp
uiLogChartGrp::uiLogChartGrp( uiParent* p, uiLogChart& lc )
    : uiGroup(p,"LogChart")
    , logchart_(lc)
{
    bgcolorfld_ = new uiColorInput( this,
				    uiColorInput::Setup(lc.backgroundColor())
				   .lbltxt(tr("Background Color")) );

    scalefld_ = new uiGenInput( this, tr("Scale Type"),
				StringListInpSpec(uiWellCharts::ScaleDef()) );
    scalefld_->attach( rightOf, bgcolorfld_ );
    scalefld_->setValue( lc.getScale() );

    uiValueAxis* zaxis = lc.getZAxis();
    majorzgridfld_ = new uiGridStyleGrp( this, tr("Major Z Grid Step"), false );
    majorzgridfld_->attach( alignedBelow, bgcolorfld_ );
    majorzgridfld_->setStyle( zaxis->getGridStyle() );
    majorzgridfld_->setSteps( zaxis->getTickInterval() );
    majorzgridfld_->setVisible( zaxis->gridVisible() );

    minorzgridfld_ = new uiGridStyleGrp( this, tr("Minor Z Grid Count"), true );
    minorzgridfld_->attach( rightAlignedBelow, majorzgridfld_ );
    minorzgridfld_->setStyle( zaxis->getMinorGridStyle() );
    minorzgridfld_->setSteps( zaxis->getMinorTickCount() );
    minorzgridfld_->setVisible( zaxis->minorGridVisible() );

    uiChartAxis* laxis = lc.logcurves()[0]->getAxis();
    majorloggridfld_ =
		new uiGridStyleGrp( this, tr("Major Log Grid Count"), true );
    majorloggridfld_->attach( rightAlignedBelow, minorzgridfld_ );
    majorloggridfld_->setStyle( laxis->getGridStyle() );
    majorloggridfld_->setSteps( laxis->getTickCount() );
    majorloggridfld_->setVisible( laxis->gridVisible() );

    minorloggridfld_ =
		new uiGridStyleGrp( this, tr("Minor Log Grid Count"), true );
    minorloggridfld_->attach( rightAlignedBelow, majorloggridfld_ );
    minorloggridfld_->setStyle( laxis->getMinorGridStyle() );
    minorloggridfld_->setSteps( laxis->getMinorTickCount() );
    minorloggridfld_->setVisible( laxis->minorGridVisible() );

    mAttachCB( bgcolorfld_->colorChanged, uiLogChartGrp::bgColorChgCB );
    mAttachCB( scalefld_->valuechanged, uiLogChartGrp::scaleChgCB );
    mAttachCB( majorzgridfld_->changed, uiLogChartGrp::zgridChgCB );
    mAttachCB( minorzgridfld_->changed, uiLogChartGrp::zgridChgCB );
    mAttachCB( majorloggridfld_->changed, uiLogChartGrp::lgridChgCB );
    mAttachCB( minorloggridfld_->changed, uiLogChartGrp::lgridChgCB );
}


uiLogChartGrp::~uiLogChartGrp()
{
    detachAllNotifiers();
}


void uiLogChartGrp::bgColorChgCB( CallBacker* )
{
    logchart_.setBackgroundColor( bgcolorfld_->color() );
}


void uiLogChartGrp::zgridChgCB( CallBacker* )
{
    uiValueAxis* zaxis = logchart_.getZAxis();
    zaxis->setTickInterval( majorzgridfld_->getSteps() );
    zaxis->setGridStyle( majorzgridfld_->getStyle() );
    zaxis->setGridLineVisible( majorzgridfld_->isVisible() );

    zaxis->setMinorTickCount( minorzgridfld_->getSteps() );
    zaxis->setMinorGridStyle( minorzgridfld_->getStyle() );
    zaxis->setMinorGridLineVisible( minorzgridfld_->isVisible() );
}


void uiLogChartGrp::lgridChgCB( CallBacker* )
{
    for ( auto* logcurve : logchart_.logcurves() )
    {
	uiChartAxis* laxis = logcurve->getAxis();
	laxis->setTickCount( majorloggridfld_->getSteps() );
	laxis->setGridStyle( majorloggridfld_->getStyle() );
	laxis->setGridLineVisible( majorloggridfld_->isVisible() );

	laxis->setMinorTickCount( minorloggridfld_->getSteps() );
	laxis->setMinorGridStyle( minorloggridfld_->getStyle() );
	laxis->setMinorGridLineVisible( minorloggridfld_->isVisible() );
    }
}


void uiLogChartGrp::scaleChgCB( CallBacker* )
{
    const uiWellCharts::Scale scaletyp =
	uiWellCharts::ScaleDef().getEnumForIndex( scalefld_->getIntValue() );
    logchart_.setScale( scaletyp );
    uiChartAxis* laxis = logchart_.logcurves()[0]->getAxis();
    minorloggridfld_->setSteps( laxis->getMinorTickCount() );
    majorloggridfld_->setSteps( laxis->getTickCount() );
    majorloggridfld_->setStepSensitive( scaletyp==uiWellCharts::Linear );
    minorloggridfld_->setStepSensitive( scaletyp==uiWellCharts::Linear );
}


// uiLogsGrp
uiLogsGrp::uiLogsGrp( uiParent* p, uiLogChart& lc )
    : uiGroup(p,"Logs")
    , logchart_(lc)
{
    setFrame( true );
    logselfld_ = new uiListBox( this, "Logs", OD::ChooseOnlyOne );
    logselfld_->setAllowNoneChosen( false );
    logselfld_->attach( leftBorder, 5 );

    logpropfld_ = new uiLogCurveProps( this, lc );
    logpropfld_->attach( rightOf, logselfld_, 5 );
    update();
    mAttachCB( logselfld_->selectionChanged, uiLogsGrp::logselCB );
}


uiLogsGrp::~uiLogsGrp()
{
    detachAllNotifiers();
}


void uiLogsGrp::update()
{
    logselfld_->setEmpty();
    auto logcurves = logchart_.logcurves();
    for ( auto* logcurve : logcurves )
    {
	BufferString str( logcurve->logName() );
	str.add(" (").add(logcurve->wellName()).add(")");
	logselfld_->addItem( str );
    }

    logselfld_->setCurrentItem( 0 );
    logselCB( nullptr );
}


void uiLogsGrp::logselCB( CallBacker* )
{
    const int selidx = logselfld_ ? logselfld_->currentItem() : 0;
    ObjectSet<LogCurve>& logcurves = logchart_.logcurves();
    if ( logcurves.validIdx(selidx) )
	logpropfld_->setLogCurve( selidx );
}


// uiGridStyleGrp
uiGridStyleGrp::uiGridStyleGrp( uiParent* p, const uiString& lbl,
				bool ticksbycount )
    : uiGroup(p,"GridStyle")
    , changed(this)
    , ticksbycount_(ticksbycount)
{
    if ( ticksbycount_ )
	stepsfld_ = new uiGenInput( this, lbl, IntInpSpec(5,1,10) );
    else
	stepsfld_ = new uiGenInput( this, lbl, FloatInpSpec(100,0,1000) );

    stepsfld_->setWithCheck();

    linestylefld_ = new uiSelLineStyle( this, OD::LineStyle() );
    linestylefld_->attach( rightOf, stepsfld_ );

    setHAlignObj( stepsfld_ );

    mAttachCB( stepsfld_->valuechanged, uiGridStyleGrp::stepChgCB );
    mAttachCB( stepsfld_->checked, uiGridStyleGrp::stepCheckedCB );
    mAttachCB( linestylefld_->changed, uiGridStyleGrp::lineStyleChgCB );
}


uiGridStyleGrp::~uiGridStyleGrp()
{
    detachAllNotifiers();
}


bool uiGridStyleGrp::isVisible() const
{
    return stepsfld_->isChecked();
}


void uiGridStyleGrp::setVisible( bool yn )
{
    stepsfld_->setChecked( yn );
    stepCheckedCB( nullptr );
}


void uiGridStyleGrp::setStepSensitive( bool yn )
{
    stepsfld_->setSensitive( yn, 0, 0 );
}

void uiGridStyleGrp::setStyle( const OD::LineStyle& ls )
{
    linestylefld_->setStyle( ls );
    lineStyleChgCB( nullptr );
}


OD::LineStyle uiGridStyleGrp::getStyle() const
{
    return linestylefld_->getStyle();
}


void uiGridStyleGrp::setSteps( int steps )
{
    stepsfld_->setValue( steps );
}


int uiGridStyleGrp::getSteps() const
{
    return stepsfld_->getIntValue();
}


void uiGridStyleGrp::stepChgCB( CallBacker* )
{
    changed.trigger();
}


void uiGridStyleGrp::stepCheckedCB( CallBacker* )
{
    linestylefld_->setSensitive( stepsfld_->isChecked() );
    changed.trigger();
}


void uiGridStyleGrp::lineStyleChgCB( CallBacker* )
{
    stepsfld_->setChecked( linestylefld_->getType()!=OD::LineStyle::None );
    stepCheckedCB( nullptr );
}
