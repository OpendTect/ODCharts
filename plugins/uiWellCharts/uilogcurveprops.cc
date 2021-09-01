/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		July 2021
________________________________________________________________________

-*/


#include "uilogcurveprops.h"

#include "datainpspec.h"
#include "draw.h"
#include "logcurve.h"
#include "uichartfillx.h"
#include "uicolor.h"
#include "uigeninput.h"
#include "uilogchart.h"
#include "uisellinest.h"
#include "wellchartcommon.h"


// uiLogFillProps
uiLogFillProps::uiLogFillProps( uiParent* p, const uiString& lbl,
				uiLogChart& lc )
    : uiGroup(p)
    , logchart_(lc)
{
    filltypefld_ = new uiGenInput( this, lbl,
			StringListInpSpec(uiChartFillx::FillTypeDef()) );

    filllimitfld_ = new uiGenInput( this, tr("Limit"),
			StringListInpSpec(uiWellCharts::FillLimitDef()));
    filllimitfld_->attach( rightOf, filltypefld_ );

    fillcolorfld_ = new uiColorInput( this,
			uiColorInput::Setup(OD::Color::Red()).withdesc(false) );
    fillcolorfld_->attach( rightOf, filllimitfld_ );

    fillbasefld_ = new uiGenInput( this, tr("BaseLine"), FloatInpSpec() );
    fillbasefld_->attach( rightOf, fillcolorfld_ );

    fillseriesfld_ = new uiGenInput( this, tr("Log"), StringListInpSpec() );
    fillseriesfld_->attach( rightOf, fillcolorfld_ );

    setHAlignObj( filltypefld_ );

    mAttachCB( filltypefld_->valuechanged, uiLogFillProps::fillTypeChgCB );
    mAttachCB( filllimitfld_->valuechanged, uiLogFillProps::fillLimitChgCB );
    mAttachCB( fillcolorfld_->colorChanged, uiLogFillProps::fillColorChgCB );
    mAttachCB( fillbasefld_->valuechanged, uiLogFillProps::fillBaseChgCB );
    mAttachCB( fillseriesfld_->valuechanged, uiLogFillProps::fillSeriesChgCB );

    fillTypeChgCB( nullptr );
    fillLimitChgCB( nullptr );
}


uiLogFillProps::~uiLogFillProps()
{
    detachAllNotifiers();
}


void uiLogFillProps::setLogFill( int lcidx, uiChartFillx::FillDir fdir )
{
    logcurve_ = nullptr;
    fill_ = nullptr;
    ObjectSet<LogCurve>& logcurves = logchart_.logcurves();
    if ( !logcurves.validIdx(lcidx) )
	return;

    logcurve_ = logcurves[lcidx];
    fill_ = fdir==uiChartFillx::Left ? logcurve_->leftFill()
				     : logcurve_->rightFill();
    if ( !fill_ )
	return;

    const float baseval = fill_->baseLineValue();
    const bool hascurve = fill_->hasBaseLineSeries();
    filltypefld_->setValue( fill_->fillType() );
    const uiWellCharts::FillLimit flim =
		hascurve ? uiWellCharts::Curve
			 : (mIsUdf(baseval) ? uiWellCharts::Track
					    : uiWellCharts::Baseline);
    filllimitfld_->setValue( flim );
    fillcolorfld_->setColor( fill_->color() );
    fillbasefld_->setValue( baseval );
    BufferStringSet lognms = logchart_.getDispLogsForID( logcurve_->wellID() );
    lognms.remove( logcurve_->logName() );
    fillseriesfld_->newSpec( StringListInpSpec(lognms), 0 );

    fillTypeChgCB( nullptr );
}


void uiLogFillProps::fillTypeChgCB( CallBacker* )
{
    const auto filltype =
		sCast(uiChartFillx::FillType,filltypefld_->getIntValue());
    const bool isnone = filltype==uiChartFillx::None;
    filllimitfld_->display( !isnone );
    fillcolorfld_->display( filltype==uiChartFillx::Color);
    if ( isnone )
	filllimitfld_->setValue( uiWellCharts::Track );

    if ( fill_ )
	fill_->setFillType( filltype, filltype!=fill_->fillType() );

    fillLimitChgCB( nullptr );
}


void uiLogFillProps::fillLimitChgCB( CallBacker* )
{
    const uiWellCharts::FillLimit filllimit =
		sCast(uiWellCharts::FillLimit,filllimitfld_->getIntValue());

    if ( filllimit==uiWellCharts::Baseline )
    {
	fillbasefld_->display( true );
	fillBaseChgCB( nullptr );
    }
    else
	fillbasefld_->display( false );

    if ( filllimit==uiWellCharts::Curve )
    {
	const BufferString tmp( fillseriesfld_->text() );
	fillseriesfld_->display( !tmp.isEmpty() );
	fillSeriesChgCB( nullptr );
    }
    else
	fillseriesfld_->display( false );
}


void uiLogFillProps::fillColorChgCB( CallBacker* )
{
    if ( !fill_ )
	return;

    OD::Color col = fillcolorfld_->color();
    fill_->setColor( col, col!=fill_->color() );
}


void uiLogFillProps::fillBaseChgCB( CallBacker* )
{
    if ( !fill_ )
	return;

    const float baseval = fillbasefld_->getFValue();
    fill_->setBaseLine( baseval,
			!mIsEqual(baseval,fill_->baseLineValue(),mDefEpsF) );
}


void uiLogFillProps::fillSeriesChgCB( CallBacker* )
{
    if ( !fill_ )
	return;

    LogCurve* lc = logchart_.getLogCurve( logcurve_->wellID(),
					  fillseriesfld_->text() );
    logcurve_->setFillToLog( fillseriesfld_->text(),
			     fill_->fillDir()==uiChartFillx::Left );
    fill_->setBaseLine( lc->getSeries() );
}


// uiLogCurveProps
uiLogCurveProps::uiLogCurveProps( uiParent* p, uiLogChart& lc)
    : uiGroup(p)
    , logchart_(lc)
{
    FloatInpIntervalSpec fspec;
    fspec.setName("Left", 0).setName("Right", 1);
    rangefld_ = new uiGenInput( this, tr("Display Range"), fspec );
    rangefld_->setWithCheck();

    linestylefld_ = new uiSelLineStyle( this, OD::LineStyle() );
    linestylefld_->attach( alignedBelow, rangefld_ );

    leftfillfld_ = new uiLogFillProps( this, tr("Left Fill"), lc );
    leftfillfld_->attach( alignedBelow, linestylefld_ );

    rightfillfld_ = new uiLogFillProps( this, tr("Right Fill"), lc );
    rightfillfld_->attach( alignedBelow, leftfillfld_ );

    mAttachCB( rangefld_->valuechanged, uiLogCurveProps::rangeChgCB );
    mAttachCB( linestylefld_->changed, uiLogCurveProps::lineStyleChgCB );
}


uiLogCurveProps::~uiLogCurveProps()
{
    detachAllNotifiers();
}


void uiLogCurveProps::setLogCurve( int idx )
{
    ObjectSet<LogCurve>& logcurves = logchart_.logcurves();
    if ( !logcurves.validIdx(idx) )
	return;

    logcurve_ = logcurves[idx];
    if ( !logcurve_ )
	return;

    rangefld_->setValue( logcurve_->dispRange() );
    linestylefld_->setStyle( logcurve_->lineStyle() );
    leftfillfld_->setLogFill( idx, uiChartFillx::Left );
    rightfillfld_->setLogFill( idx, uiChartFillx::Right);
}


void uiLogCurveProps::lineStyleChgCB( CallBacker* )
{
    if ( logcurve_ )
	logcurve_->setLineStyle( linestylefld_->getStyle() );
}


void uiLogCurveProps::rangeChgCB( CallBacker* )
{
    if ( logcurve_ )
	logcurve_->setDisplayRange( rangefld_->getFInterval() );
}
