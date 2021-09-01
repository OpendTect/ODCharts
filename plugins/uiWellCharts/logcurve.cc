/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		August 2020
________________________________________________________________________

-*/


#include "logcurve.h"
#include "chartutils.h"

#include "uichartfillx.h"
#include "uichartaxes.h"
#include "uilogchart.h"
#include "wellchartcommon.h"

#include "draw.h"
#include "math2.h"
#include "mnemonics.h"
#include "multiid.h"
#include "ranges.h"
#include "separstr.h"
#include "welldata.h"
#include "welllog.h"
#include "wellman.h"

LogCurve::LogCurve()
    : wellid_(MultiID::udf())
    , logname_(BufferString::empty())
{}


LogCurve::LogCurve( const MultiID& wellid, const char* lognm )
    : wellid_(wellid)
    , logname_(lognm)
{
    initLog();
}


LogCurve::~LogCurve()
{
    deepErase( series_ );
    deleteAndZeroPtr( scatseries_ );
    deleteAndZeroPtr( leftfill_ );
    deleteAndZeroPtr( rightfill_ );
    deleteAndZeroPtr( axis_ );
}


bool LogCurve::initLog()
{
    Well::LoadReqs lreq( Well::LogInfos );
    RefMan<Well::Data> wd = Well::MGR().get( wellid_, lreq );
    if ( !wd || !wd->getLog(logname_) )
	return false;

    const Well::Log* log = wellLog();
    if ( !log )
	return false;

    wellname_ = wd->name();
    uomlbl_ = log->unitOfMeasure()->symbol();
    mnemlbl_ = log->mnemLabel();
    dahrange_ = log->dahRange();
    valrange_ = log->valueRange();
    disprange_.setUdf();
    addLog( *log );
    return true;
}


const Well::Log* LogCurve::wellLog() const
{
    Well::LoadReqs lreq( Well::LogInfos );
    RefMan<Well::Data> wd = Well::MGR().get( wellid_, lreq );
    return wd ? wd->getLog( logname_ ) : nullptr;
}


void LogCurve::addTo( uiLogChart& logchart )
{
    const Well::Log* log = wellLog();
    if ( !log )
	return;

    const Mnemonic* mnem = MNC().getByName( mnemlbl_ );
    OD::LineStyle lstyle;
    float min, max;
    bool reverse;
    if ( mnem )
    {
	const Mnemonic::DispDefs& disp = mnem->disp_;
	const UnitOfMeasure* mnem_uom = UoMR().get( disp.getUnitLbl() );
	const UnitOfMeasure* log_uom = log->unitOfMeasure();

	disprange_.start = getConvertedValue( disp.typicalrange_.start,
					      mnem_uom, log_uom );
	disprange_.stop = getConvertedValue( disp.typicalrange_.stop,
					      mnem_uom, log_uom );
	lstyle.color_ = disp.color_;
    }
    else
    {
	disprange_.start = valrange_.start;
	disprange_.stop = valrange_.stop;
    }

    if ( !disprange_.isUdf() )
    {
	min = disprange_.isRev() ? disprange_.stop : disprange_.start;
	max = disprange_.isRev() ? disprange_.start : disprange_.stop;
	reverse = disprange_.isRev();
	addTo( logchart, lstyle, min, max, reverse );
    }
}


void LogCurve::addTo( uiLogChart& logchart, const IOPar& iop )
{
    usePar( iop );
    float min = disprange_.isRev() ? disprange_.stop : disprange_.start;
    float max = disprange_.isRev() ? disprange_.start : disprange_.stop;
    bool reverse = disprange_.isRev();
    addTo( logchart, linestyle_, min, max, reverse );
}


void LogCurve::addTo( uiLogChart& logchart, const OD::LineStyle& lstyle )
{
    addTo( logchart, lstyle, valrange_.start, valrange_.stop, false );
}


void LogCurve::addTo( uiLogChart& logchart, const OD::LineStyle& lstyle,
		      float min, float max, bool reverse )
{
    BufferString logtitle( logname_ );
    logtitle.add(" - ").add( wellname_ );
    logtitle.add(" (").add( uomlbl_ ).add(")");
    StepInterval<float> ni =
		StepInterval<float>( min, max, 1 ).niceInterval( 10, false );

    axis_ = logchart.makeLogAxis( logtitle, ni.start, ni.stop, reverse );
    axis_->setLineStyle( lstyle );
    disprange_ = axis_->range();
    logchart.addAxis( axis_, OD::Top );
    logchart.getZAxis()->setAxisLimits( dahrange_ );
    BufferString callouttxt( wellname_, "\nDepth: %2\n", logname_ );
    callouttxt.add( ": %1" );
    linestyle_ = lstyle;
    for ( auto* series : series_ )
    {
	series->setLineStyle( lstyle );
	logchart.addSeries( series );
	series->attachAxis( logchart.getZAxis() );
	series->attachAxis( axis_ );
	series->setCalloutTxt( callouttxt );
    }

    if ( scatseries_ )
    {
	scatseries_->setColor( lstyle.color_ );
	scatseries_->setBorderColor( lstyle.color_ );
	scatseries_->setMarkerSize( pointsize_ );
	logchart.addSeries( scatseries_ );
	scatseries_->attachAxis( logchart.getZAxis() );
	scatseries_->attachAxis( axis_ );
	scatseries_->setCalloutTxt( callouttxt );
    }

    if ( !leftfill_ )
	leftfill_ = new uiChartFillx;
    if ( !rightfill_ )
	rightfill_ = new uiChartFillx;

    leftfill_->addTo( &logchart, series_ );
    rightfill_->addTo( &logchart, series_, uiChartFillx::Right );
}


void LogCurve::addCurveFillTo( uiLogChart& logchart )
{
    if ( !lefttolog_.isEmpty() )
    {
	LogCurve* lc = logchart.getLogCurve( wellID(), lefttolog_ );
	if ( lc )
	    leftfill_->setBaseLine( lc->getSeries(), false );
    }

    if ( !righttolog_.isEmpty() )
    {
	LogCurve* lc = logchart.getLogCurve( wellID(), righttolog_ );
	if ( lc )
	    rightfill_->setBaseLine( lc->getSeries(), false );
    }
}


void LogCurve::addLog( const Well::Log& log )
{
    series_.setEmpty();
    deleteAndZeroPtr( scatseries_ );

    auto* series = new uiLineSeries();
    const UnitOfMeasure* zdisp_uom = UnitOfMeasure::surveyDefDepthUnit();
    const UnitOfMeasure* zstor_uom = UnitOfMeasure::surveyDefDepthStorageUnit();
    dahrange_.start = getConvertedValue(dahrange_.start, zstor_uom, zdisp_uom);
    dahrange_.stop = getConvertedValue( dahrange_.stop, zstor_uom, zdisp_uom );

    for (int idx=0; idx<log.size(); idx++)
    {
	const float logval = log.value(idx);
	const float dah = getConvertedValue( log.dah( idx ), zstor_uom,
					     zdisp_uom );
	if ( !mIsUdf(logval) )
	    series->append( logval, dah );
	else
	{
	    if ( series->size()==1 )
	    {
		if ( !scatseries_ )
		    scatseries_ = new uiScatterSeries();
		scatseries_->append( series->x(0), series->y(0) );
		series->clear();
	    }
	    else if ( !series->isEmpty() )
	    {
		series_ += series;
		series = new uiLineSeries();
	    }
	}
    }

    if ( !series->isEmpty() )
	series_ += series;
    else
	delete series;

    leftfill_ = new uiChartFillx();
    rightfill_ = new uiChartFillx();
    lefttolog_.setEmpty();
    righttolog_.setEmpty();
}


void LogCurve::removeFrom( uiLogChart& logchart )
{
    for ( auto* series : series_ )
	logchart.removeSeries( series );

    if ( scatseries_ )
	logchart.removeSeries( scatseries_ );

    deleteAndZeroPtr( leftfill_ );
    deleteAndZeroPtr( rightfill_ );
    lefttolog_.setEmpty();
    righttolog_.setEmpty();

    logchart.removeAxis( axis_ );
}


void LogCurve::setLineStyle(const OD::LineStyle& ls, bool usetransp)
{
    linestyle_ = ls;
    for ( auto* series : series_ )
    {
	series->setLineStyle( linestyle_, usetransp );
    }

    if ( scatseries_ )
    {
	scatseries_->setColor( ls.color_ );
	scatseries_->setBorderColor( ls.color_ );
    }

    if ( axis_ )
	axis_->setLineStyle( linestyle_ );
}


OD::LineStyle LogCurve::lineStyle() const
{
    return linestyle_;
}


void LogCurve::setDisplayRange( float left, float right )
{
    setDisplayRange( Interval<float>(left,right) );
}


void LogCurve::setDisplayRange( const Interval<float>& range )
{
    disprange_ = range;
    if ( axis_ )
	axis_->setRange( range );
}


void LogCurve::setFillToLog( const char* lognm, bool left )
{
    (left ? lefttolog_ : righttolog_) = lognm;
}


void LogCurve::fillPar( IOPar& par ) const
{
    par.set( sKey::ID(), wellid_ );
    par.set( sKey::Log(), logname_);
    par.set( sKey::Range(), disprange_ );

    BufferString lsstr;
    lineStyle().toString( lsstr );
    par.set( sKey::LineStyle(), lsstr );

    par.set( uiChartFillx::toString(uiChartFillx::Left), getFillPar(true) );
    par.set( uiChartFillx::toString(uiChartFillx::Right), getFillPar(false) );
}


BufferString LogCurve::getFillPar( bool left ) const
{
    BufferString res;
    const BufferString tolognm = left ? lefttolog_ : righttolog_;
    uiChartFillx* fill = left ? leftfill_ : rightfill_;

    FileMultiString fms = uiChartFillx::toString( fill->fillType() );
    const float baseval = fill->baseLineValue();
    const bool hascurve = fill->hasBaseLineSeries();
    const uiWellCharts::FillLimit flim =
		hascurve ? uiWellCharts::Curve
			 : (mIsUdf(baseval) ? uiWellCharts::Track
					    : uiWellCharts::Baseline);
    fms += toString( flim );

    if ( flim==uiWellCharts::Baseline )
	fms += baseval;
    else if ( flim==uiWellCharts::Curve )
	fms += tolognm;

    if ( fill->fillType()==uiChartFillx::Color )
    {
	BufferString col;
	fill->color().fill( col );
	fms += FileMultiString( col );
    }

    res = fms;
    return res;
}


void LogCurve::setFillPar( const char* fillstr, bool left )
{
    uiChartFillx* fill = left ? leftfill_ : rightfill_;
    fill->setBaseLine( mUdf(float) );

    FileMultiString fms( fillstr );
    uiChartFillx::FillType ftype;
    uiChartFillx::parseEnum( fms[0], ftype );
    fill->setFillType( ftype, false );

    uiWellCharts::FillLimit flim;
    uiWellCharts::parseEnum( fms[1], flim );

    int next = 2;
    if ( flim==uiWellCharts::Baseline )
    {
	fill->setBaseLine( fms.getFValue(next), false );
	next++;
    }
    else if ( flim==uiWellCharts::Curve )
    {
	setFillToLog( fms[next], left );
	next++;
    }

    if ( ftype==uiChartFillx::Color )
    {
	FileMultiString colfms( fms.from(next) );
	OD::Color col;
	col.use( colfms );
	fill->setColor( col, false );
    }
}


void LogCurve::usePar( const IOPar& par )
{
    par.get( sKey::ID(), wellid_ );
    par.get( sKey::Log(), logname_ );
    initLog();
    Interval<float> range;
    range.setUdf();
    par.get( sKey::Range(), range );
    setDisplayRange( range );

    BufferString lsstr;
    par.get( sKey::LineStyle(), lsstr );
    linestyle_.fromString( lsstr );

    BufferString fillstr;
    par.get( uiChartFillx::toString(uiChartFillx::Left), fillstr );
    setFillPar( fillstr, true );
    par.get( uiChartFillx::toString(uiChartFillx::Right), fillstr );
    setFillPar( fillstr, false );
}
