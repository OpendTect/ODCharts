/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		July 2021
________________________________________________________________________

-*/


#include "uilogcurveprops.h"

#include "uichartfillx.h"
#include "uicolor.h"
#include "uicolortable.h"
#include "uigeninput.h"
#include "uilogchart.h"
#include "uisellinest.h"

#include "draw.h"
#include "logcurve.h"
#include "loggradient.h"
#include "wellchartcommon.h"
#include "wellman.h"


// uiLogFillProps
uiLogFillProps::uiLogFillProps( uiParent* p, uiChartFillx::FillDir fdir,
				uiLogChart& lc )
    : uiGroup(p)
    , logchart_(lc)
    , filldir_(fdir)
{
    const EnumDef& ftdef = uiChartFillx::FillTypeDef();
    const EnumDef& fldef = uiWellCharts::FillLimitDef();
    uiString lbl1( tr("%1 Fill Type").arg(uiChartFillx::toUiString(fdir)) );
    uiString lbl2( tr("%1 Fill Limit").arg(uiChartFillx::toUiString(fdir)) );

    filltypefld_ = new uiGenInput( this, lbl1, StringListInpSpec(ftdef) );

    fillcolorfld_ = new uiColorInput( this,
			uiColorInput::Setup(Color::Red()).withdesc(false) );
    fillcolorfld_->attach( rightOf, filltypefld_ );

    gradientlogfld_ = new uiGenInput( this, uiString::empty(),
				      StringListInpSpec() );
    gradientlogfld_->attach( rightOf, filltypefld_ );

    fillgradientfld_ = new uiColorTableGroup( this, OD::Horizontal, false );
    fillgradientfld_->attach( rightOf, gradientlogfld_ );
    fillgradientfld_->enableManage( false );
    fillgradientfld_->enableClippingDlg( false );

    filllimitfld_ = new uiGenInput( this, lbl2, StringListInpSpec(fldef) );
    filllimitfld_->attach( alignedBelow, filltypefld_ );

    fillbasefld_ = new uiGenInput( this, tr("BaseLine"), FloatInpSpec() );
    fillbasefld_->attach( rightOf, filllimitfld_ );

    fillseriesfld_ = new uiGenInput( this, tr("Log"), StringListInpSpec() );
    fillseriesfld_->attach( rightOf, filllimitfld_ );

    setHAlignObj( filltypefld_ );

    mAttachCB( filltypefld_->valuechanged, uiLogFillProps::fillTypeChgCB );
    mAttachCB( filllimitfld_->valuechanged, uiLogFillProps::fillLimitChgCB );
    mAttachCB( fillcolorfld_->colorChanged, uiLogFillProps::fillColorChgCB );
    mAttachCB( gradientlogfld_->valuechanged,
	       uiLogFillProps::fillGradientLogChgCB );
    mAttachCB( fillgradientfld_->scaleChanged,
	       uiLogFillProps::fillGradientChgCB );
    mAttachCB( fillgradientfld_->seqChanged,
	       uiLogFillProps::fillGradientChgCB );
    mAttachCB( fillbasefld_->valuechanged, uiLogFillProps::fillBaseChgCB );
    mAttachCB( fillseriesfld_->valuechanged, uiLogFillProps::fillSeriesChgCB );

    fillTypeChgCB( nullptr );
    fillLimitChgCB( nullptr );
}


uiLogFillProps::~uiLogFillProps()
{
    detachAllNotifiers();
}


void uiLogFillProps::setLogFill( int lcidx )
{
    logcurve_ = nullptr;
    fill_ = nullptr;
    ObjectSet<LogCurve>& logcurves = logchart_.logcurves();
    if ( !logcurves.validIdx(lcidx) )
	return;

    logcurve_ = logcurves[lcidx];
    fill_ = filldir_==uiChartFillx::Left ? logcurve_->leftFill()
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
    if ( lognms.isEmpty() )
    {
	uiStringSet uistr( uiWellCharts::FillLimitDef().strings() );
	int idx = uiWellCharts::FillLimitDef().indexOf( uiWellCharts::Curve );
	uistr.removeSingle( idx );
	filllimitfld_->newSpec( StringListInpSpec(uistr), 0 );
	fillseriesfld_->newSpec( StringListInpSpec(), 0 );
    }
    else
    {
	filllimitfld_->newSpec(
		StringListInpSpec(uiWellCharts::FillLimitDef()), 0 );
	filllimitfld_->setValue( flim );
	fillseriesfld_->newSpec( StringListInpSpec(lognms), 0 );
	fillseriesfld_->setValue( logcurve_->fillToLog(
					    filldir_==uiChartFillx::Left) );
    }

    Well::Man::getLogNamesByID( logcurve_->wellID(), lognms, false );
    if ( lognms.isEmpty() )
    {
	filltypefld_->setValue( uiChartFillx::ColorFill );
	gradientlogfld_->newSpec( StringListInpSpec(), 0 );
    }
    else
	gradientlogfld_->newSpec( StringListInpSpec(lognms), 0 );

    mDynamicCastGet(LogGradient*,lg,fill_->gradientImg());
    if ( lg )
    {
	gradientlogfld_->setText( lg->logName() );
	fillgradientfld_->setInterval( lg->dispRange() );
	fillgradientfld_->setSequence( lg->sequenceName() );
    }
    fillTypeChgCB( nullptr );
}


void uiLogFillProps::fillTypeChgCB( CallBacker* )
{
    const auto filltype =
		sCast(uiChartFillx::FillType,filltypefld_->getIntValue());
    const bool isnone = filltype==uiChartFillx::NoFill;
    filllimitfld_->display( !isnone );
    fillcolorfld_->display( filltype==uiChartFillx::ColorFill );
    if ( isnone )
	filllimitfld_->setValue( uiWellCharts::Track );

    const bool isgradient = filltype==uiChartFillx::GradientFill;
    gradientlogfld_->display( isgradient );
    fillgradientfld_->display( isgradient );
    if ( isgradient )
	fillGradientLogChgCB( nullptr );

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

    if ( fill_ && filllimit==uiWellCharts::Track )
    {
	fill_->setBaseLine( mUdf(float), false );
	fill_->setBaseLineSeriesEmpty();
    }
}


void uiLogFillProps::fillColorChgCB( CallBacker* )
{
    if ( !fill_ )
	return;

    Color col = fillcolorfld_->color();
    fill_->setColor( col, col!=fill_->color() );
}


void uiLogFillProps::fillGradientChgCB( CallBacker* )
{
    if ( !fill_ )
	return;

    mDynamicCastGet(LogGradient*,lg,fill_->gradientImg());
    if ( !lg )
	return;

    lg->setColTabMapperSetup( fillgradientfld_->colTabMapperSetup(), false );
    lg->setColTabSequence( fillgradientfld_->colTabSeq(), true );
    fill_->updateCB( nullptr );
}


void uiLogFillProps::fillGradientLogChgCB( CallBacker* )
{
    if ( !fill_ )
	return;

    auto* lg = new LogGradient( logcurve_->wellID(), gradientlogfld_->text() );
    if ( !lg )
	return;

    Interval<float> rg = lg->dispRange().isUdf() ? lg->logRange()
						 : lg->dispRange();
    rg.sort();
    fillgradientfld_->setInterval( rg );
    fill_->setGradientImg( lg, false );
    fillGradientChgCB( nullptr );
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
    fspec.setName("Left",0).setName("Right",1);
    rangefld_ = new uiGenInput( this, tr("Display range"), fspec );
    rangefld_->setWithCheck();

    linestylefld_ = new uiSelLineStyle( this, OD::LineStyle() );
    linestylefld_->attach( alignedBelow, rangefld_ );

    leftfillfld_ = new uiLogFillProps( this, uiChartFillx::Left, lc );
    leftfillfld_->attach( alignedBelow, linestylefld_ );

    rightfillfld_ = new uiLogFillProps( this, uiChartFillx::Right, lc );
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
    leftfillfld_->setLogFill( idx );
    rightfillfld_->setLogFill( idx );
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
