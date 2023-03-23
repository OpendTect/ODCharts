/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uilogviewpropdlg.h"

#include "draw.h"
#include "logcurve.h"
#include "markerline.h"
#include "uichartaxes.h"
#include "uicolor.h"
#include "uigeninput.h"
#include "uilistbox.h"
#include "uilogchart.h"
#include "uilogcurveprops.h"
#include "uimain.h"
#include "uisellinest.h"
#include "uitabstack.h"
#include "wellchartcommon.h"


// uiLogViewPropDlg
uiLogViewPropDlg::uiLogViewPropDlg( uiParent* p, uiLogChart* logchart,
				    bool withapply )
    : uiDialog(p,Setup(tr("Display Properties Editor"),mNoDlgTitle,
		       mTODOHelpKey).applybutton(withapply))
    , logchart_(logchart)
    , withapply_(withapply)
{
    setCtrlStyle( CloseOnly );
    setModal( false );
    setShrinkAllowed( true );

    tabs_ = new uiTabStack( this, "Properties" );
    chartgrp_ = new uiLogChartGrp( tabs_->tabGroup(), logchart_ );
    logsgrp_ = new uiLogsGrp( tabs_->tabGroup(), logchart_ );
    markersgrp_ = new uiMarkersGrp( tabs_->tabGroup(), logchart_ );

    tabs_->addTab( chartgrp_, tr("Chart properties") );
    tabs_->addTab( logsgrp_,  uiStrings::sLog(2) );
    tabs_->addTab( markersgrp_, uiStrings::sMarker(2) );

    mAttachCB(logchart_->logChange, uiLogViewPropDlg::updateLogsCB);
    mAttachCB(logchart_->markerChange, uiLogViewPropDlg::updateMarkersCB);
    if ( withapply_ )
	mAttachCB(applyPushed, uiLogViewPropDlg::applyCB);

    mAttachCB( uiMain::instance().topLevel()->windowClosed,
	       uiLogViewPropDlg::closeCB );

    logchart_->fillPar( settingsbackup_ );
}


uiLogViewPropDlg::~uiLogViewPropDlg()
{
    detachAllNotifiers();
}


void uiLogViewPropDlg::setLogChart( uiLogChart* logchart )
{
    restoreCB( nullptr );
    logchart_ = logchart;
    chartgrp_->setLogChart( logchart );
    logsgrp_->setLogChart( logchart );
    markersgrp_->setLogChart( logchart );
    settingsbackup_.setEmpty();
    logchart->fillPar( settingsbackup_ );
}


void uiLogViewPropDlg::applyCB( CallBacker* )
{
    settingsbackup_.setEmpty();
    logchart_->fillPar( settingsbackup_ );
}


IOPar uiLogViewPropDlg::getCurrentSettings() const
{
    IOPar res;
    logchart_->fillPar( res );
    return res;
}


void uiLogViewPropDlg::updateLogsCB( CallBacker* )
{
    logsgrp_->update();
}


void uiLogViewPropDlg::updateMarkersCB( CallBacker* )
{
    markersgrp_->update();
}


void uiLogViewPropDlg::restoreCB( CallBacker* )
{
    if ( settingsbackup_.isEmpty() )
	return;

    logchart_->usePar( settingsbackup_, true );
    chartgrp_->update();
    logsgrp_->update();
    markersgrp_->update();
}


bool uiLogViewPropDlg::rejectOK( CallBacker* )
{
    if ( withapply_ )
	restoreCB( nullptr );
    return true;
}


void uiLogViewPropDlg::closeCB( CallBacker* )
{
    close();
}


// uiLogChartGrp
uiLogChartGrp::uiLogChartGrp( uiParent* p, uiLogChart* lc )
    : uiGroup(p,"LogChart")
    , logchart_(lc)
{
    bgcolorfld_ = new uiColorInput( this,
				    uiColorInput::Setup(lc->backgroundColor())
				   .lbltxt(tr("Background Color")) );
    bgcolorfld_->setStretch( 0, 0 );

    scalefld_ = new uiGenInput( this, tr("Scale Type"),
				StringListInpSpec(uiWellCharts::ScaleDef()) );
    scalefld_->attach( rightOf, bgcolorfld_ );

    majorzgridfld_ = new uiGridStyleGrp( this, tr("Major Z Grid Step"), false );
    majorzgridfld_->attach( ensureBelow, bgcolorfld_ );

    minorzgridfld_ = new uiGridStyleGrp( this, tr("Minor Z Grid Count"), true );
    minorzgridfld_->attach( alignedBelow, majorzgridfld_ );

    majorloggridfld_ =
		new uiGridStyleGrp( this, tr("Major Log Grid Count"), true );
    majorloggridfld_->attach( alignedBelow, minorzgridfld_ );

    minorloggridfld_ =
		new uiGridStyleGrp( this, tr("Minor Log Grid Count"), true );
    minorloggridfld_->attach( alignedBelow, majorloggridfld_ );

    update();
    mAttachCB( bgcolorfld_->colorChanged, uiLogChartGrp::bgColorChgCB );
    mAttachCB( scalefld_->valueChanged, uiLogChartGrp::scaleChgCB );
    mAttachCB( majorzgridfld_->changed, uiLogChartGrp::zgridChgCB );
    mAttachCB( minorzgridfld_->changed, uiLogChartGrp::zgridChgCB );
    mAttachCB( majorloggridfld_->changed, uiLogChartGrp::lgridChgCB );
    mAttachCB( minorloggridfld_->changed, uiLogChartGrp::lgridChgCB );
}


uiLogChartGrp::~uiLogChartGrp()
{
    detachAllNotifiers();
}


void uiLogChartGrp::setLogChart( uiLogChart* logchart )
{
    logchart_ = logchart;
    update();
}


void uiLogChartGrp::update()
{
    bgcolorfld_->setColor( logchart_->backgroundColor() );
    scalefld_->setValue( logchart_->getScale() );

    uiValueAxis* zaxis = logchart_->getZAxis();
    majorzgridfld_->setStyle( zaxis->getGridStyle() );
    majorzgridfld_->setSteps( zaxis->getTickInterval() );
    majorzgridfld_->setVisible( zaxis->gridVisible() );
    minorzgridfld_->setStyle( zaxis->getMinorGridStyle() );
    minorzgridfld_->setSteps( zaxis->getMinorTickCount() );
    minorzgridfld_->setVisible( zaxis->minorGridVisible() );

    uiChartAxis* laxis = logchart_->logcurves()[0]->getAxis();
    majorloggridfld_->setStyle( laxis->getGridStyle() );
    majorloggridfld_->setSteps( laxis->getTickCount() );
    majorloggridfld_->setVisible( laxis->gridVisible() );
    minorloggridfld_->setStyle( laxis->getMinorGridStyle() );
    minorloggridfld_->setSteps( laxis->getMinorTickCount() );
    minorloggridfld_->setVisible( laxis->minorGridVisible() );
}


void uiLogChartGrp::bgColorChgCB( CallBacker* )
{
    logchart_->setBackgroundColor( bgcolorfld_->color() );
}


void uiLogChartGrp::zgridChgCB( CallBacker* )
{
    uiValueAxis* zaxis = logchart_->getZAxis();
    zaxis->setTickInterval( majorzgridfld_->getSteps() );
    zaxis->setGridStyle( majorzgridfld_->getStyle() );
    zaxis->setGridLineVisible( majorzgridfld_->isVisible() );

    zaxis->setMinorTickCount( minorzgridfld_->getSteps() );
    zaxis->setMinorGridStyle( minorzgridfld_->getStyle() );
    zaxis->setMinorGridLineVisible( minorzgridfld_->isVisible() );
}


void uiLogChartGrp::lgridChgCB( CallBacker* )
{
    for ( auto* logcurve : logchart_->logcurves() )
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
    logchart_->setScale( scaletyp );
    uiChartAxis* laxis = logchart_->logcurves()[0]->getAxis();
    minorloggridfld_->setSteps( laxis->getMinorTickCount() );
    majorloggridfld_->setSteps( laxis->getTickCount() );
    majorloggridfld_->setStepSensitive( scaletyp==uiWellCharts::Linear );
    minorloggridfld_->setStepSensitive( scaletyp==uiWellCharts::Linear );
}


// uiLogsGrp
uiLogsGrp::uiLogsGrp( uiParent* p, uiLogChart* lc )
    : uiGroup(p,"Logs")
    , logchart_(lc)
{
    logselfld_ = new uiListBox( this, "Logs", OD::ChooseOnlyOne );
    logselfld_->setHSzPol( uiObject::Wide );
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


void uiLogsGrp::setLogChart( uiLogChart* logchart )
{
    logchart_ = logchart;
    logpropfld_->setLogChart( logchart );
    update();
}


void uiLogsGrp::update()
{
    logselfld_->setEmpty();
    auto wellnames = logchart_->wellNames();
    auto logcurves = logchart_->logcurves();
    for ( auto* logcurve : logcurves )
    {
	BufferString str( logcurve->logName() );
	if ( wellnames.size()>1 )
	    str.add(" (").add(logcurve->wellName()).add(")");
	logselfld_->addItem( str );
    }

    logselfld_->setCurrentItem( 0 );
    logselCB( nullptr );
}


void uiLogsGrp::logselCB( CallBacker* )
{
    const int selidx = logselfld_ ? logselfld_->currentItem() : 0;
    ObjectSet<LogCurve>& logcurves = logchart_->logcurves();
    if ( logcurves.validIdx(selidx) )
	logpropfld_->setLogCurve( selidx );
}


// uiMarkersGrp
uiMarkersGrp::uiMarkersGrp( uiParent* p, uiLogChart* lc )
    : uiGroup(p,"Markers")
    , logchart_(lc)
{
    markerselfld_ = new uiListBox( this, "Markers", OD::ChooseOnlyOne );
    markerselfld_->setHSzPol( uiObject::Wide );
    markerselfld_->setAllowNoneChosen( false );
    markerselfld_->attach( leftBorder, 5 );

    markerlinefld_ = new uiSelLineStyle( this, OD::LineStyle() );
    markerlinefld_->attach( rightOf, markerselfld_ );
    update();
    mAttachCB( markerselfld_->selectionChanged, uiMarkersGrp::markerselCB );
    mAttachCB( markerlinefld_->changed, uiMarkersGrp::lineStyleChgCB );
}


uiMarkersGrp::~uiMarkersGrp()
{
    detachAllNotifiers();
}


void uiMarkersGrp::setLogChart( uiLogChart* logchart )
{
    logchart_ = logchart;
    update();
}


void uiMarkersGrp::update()
{
    markerselfld_->setEmpty();
    BufferStringSet wellnames = logchart_->wellNames();
    const ObjectSet<MarkerLine>& markers = logchart_->markers();
    for ( auto* marker : markers )
    {
	BufferString str( marker->markerName() );
	if ( wellnames.size()>1 )
	    str.add(" (").add(marker->wellName()).add(")");
	markerselfld_->addItem( str );
    }

    markerselfld_->setCurrentItem( 0 );
    markerselCB( nullptr );
}


void uiMarkersGrp::markerselCB( CallBacker* )
{
    const int selidx = markerselfld_ ? markerselfld_->currentItem() : 0;
    ObjectSet<MarkerLine>& markers = logchart_->markers();
    if ( markers.validIdx(selidx) )
	markerlinefld_->setStyle( markers[selidx]->lineStyle() );
}


void uiMarkersGrp::lineStyleChgCB( CallBacker* )
{
    const int selidx = markerselfld_->currentItem();
    ObjectSet<MarkerLine>& markers = logchart_->markers();
    if ( markers.validIdx(selidx) )
	markers[selidx]->setLineStyle( markerlinefld_->getStyle() );
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

    setHAlignObj( linestylefld_ );

    mAttachCB( stepsfld_->valueChanged, uiGridStyleGrp::stepChgCB );
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
