/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		August 2020
________________________________________________________________________

-*/


#include "logcurve.h"
#include "chartutils.h"

#include "loggradient.h"
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
    : LogData()
{}


LogCurve::LogCurve( const MultiID& wellid, const char* lognm )
    : LogData(wellid,lognm)
{
    const Well::Log* log = wellLog();
    if ( log )
	addLog( *log );
}


LogCurve::LogCurve( const char* wellnm, const Well::Log& log )
    : LogData()
{
    initLog( wellnm, log );
    addLog( log );
}


LogCurve::~LogCurve()
{
    deepErase( series_ );
    deleteAndZeroPtr( scatseries_ );
    deleteAndZeroPtr( leftfill_ );
    deleteAndZeroPtr( rightfill_ );
    deleteAndZeroPtr( axis_ );
}


void LogCurve::initCallBacks()
{
    if ( scatseries_ )
	scatseries_->initCallBacks();

    for ( auto* series : series_ )
	series->initCallBacks();
}


LogCurve* LogCurve::clone() const
{
    auto* lc = new LogCurve;
    lc->wellid_ = wellid_;
    lc->wellname_ = wellname_;
    lc->uomlbl_ = uomlbl_;
    lc->logname_ = logname_;
    lc->mnemlbl_ = mnemlbl_;
    lc->dahrange_ = dahrange_;
    lc->valrange_ = valrange_;
    lc->disprange_ = disprange_;
    lc->linestyle_ = linestyle_;
    lc->pointsize_ = pointsize_;
    lc->axis_ = axis_;
    lc->leftfill_ = leftfill_;
    lc->rightfill_ = rightfill_;
    lc->lefttolog_ = lefttolog_;
    lc->righttolog_ = righttolog_;
    if ( scatseries_ )
    {
	lc->scatseries_ = new uiScatterSeries;
	lc->scatseries_->copyPoints( *scatseries_ );
    }

    for ( const auto* series : series_ )
    {
	auto* ns = new uiLineSeries;
	ns->copyPoints( *series );
	lc->series_ += ns;
    }

    return lc;
}


void LogCurve::addTo( uiLogChart& logchart, bool show_wellnm, bool show_uom )
{
    const Well::Log* log = wellLog();
    if ( !log )
	return;

    const Mnemonic* mnem = MNC().getByName( mnemlbl_ );
    OD::LineStyle lstyle;
    if ( mnem )
    {
	const Mnemonic::DispDefs& disp = mnem->disp_;
	lstyle.color_ = disp.color_;
    }

    addTo( logchart, lstyle, show_wellnm, show_uom );
}


void LogCurve::addTo( uiLogChart& logchart, const IOPar& iop )
{
    usePar( iop );
    addTo( logchart, linestyle_ );
}


void LogCurve::addTo( uiLogChart& logchart, const OD::LineStyle& lstyle,
		      bool show_wellnm, bool show_uom )
{
    const auto& dr = dispRange();
    const bool reverse = dr.isRev();
    const float min = reverse ? dr.stop : dr.start;
    const float max = reverse ? dr.start : dr.stop;
    addTo( logchart, lstyle, min, max, reverse, show_wellnm, show_uom );
}


void LogCurve::addTo( uiLogChart& logchart, const OD::LineStyle& lstyle,
		      float min, float max, bool reverse, bool show_wellnm,
		      bool show_uom )
{
    initCallBacks();
    BufferString logtitle( logname_ );
    if ( show_wellnm )
	logtitle.add(" - ").add( wellname_ );
    if ( show_uom )
	logtitle.add(" (").add( uomlbl_ ).add(")");

    axis_ = logchart.makeLogAxis( logtitle, min, max, reverse );
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
    deleteAndZeroPtr( leftfill_ );
    deleteAndZeroPtr( rightfill_ );
    deleteAndZeroPtr( axis_ );

    auto* series = new uiLineSeries;
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
		    scatseries_ = new uiScatterSeries;
		scatseries_->append( series->x(0), series->y(0) );
		series->clear();
	    }
	    else if ( !series->isEmpty() )
	    {
		series_ += series;
		series = new uiLineSeries;
	    }
	}
    }

    if ( !series->isEmpty() )
	series_ += series;
    else
	delete series;

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
    deleteAndZeroPtr( axis_ );
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


void LogCurve::setFillToLog( const char* lognm, bool left )
{
    (left ? lefttolog_ : righttolog_) = lognm;
}


const char* LogCurve::fillToLog( bool left )
{
    return left ? lefttolog_ : righttolog_;
}


void LogCurve::setDisplayRange( const Interval<float>& range )
{
    LogData::setDisplayRange( range );
    if ( axis_ )
	axis_->setRange( dispRange() );
}


void LogCurve::fillPar( IOPar& par ) const
{
    LogData::fillPar( par );

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

    if ( fill->fillType()==uiChartFillx::ColorFill )
    {
	BufferString col;
	fill->color().fill( col );
	fms += FileMultiString( col );
    }
    else if ( fill->fillType()==uiChartFillx::GradientFill )
	fms += fill->gradientImg()->toString();

    res = fms;
    return res;
}


void LogCurve::setFillPar( const char* fillstr, bool left )
{
    if ( !leftfill_ )
	leftfill_ = new uiChartFillx();
    if ( !rightfill_ )
	rightfill_ = new uiChartFillx();

    uiChartFillx* fill = left ? leftfill_ : rightfill_;
    fill->setBaseLine( mUdf(float), false );

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

    if ( ftype==uiChartFillx::ColorFill )
    {
	FileMultiString colfms( fms.from(next) );
	OD::Color col;
	col.use( colfms );
	fill->setColor( col, false );
    }
    else if ( ftype==uiChartFillx::GradientFill )
    {
	FileMultiString gradfms( fms.from(next) );
	LogGradient* lg = new LogGradient( wellid_ );
	lg->fromString( gradfms );
	fill->setGradientImg( lg, false );
    }
}


void LogCurve::usePar( const IOPar& par )
{
    LogData::usePar( par );
    const Well::Log* log = wellLog();
    if ( log )
	addLog( *log );


    BufferString lsstr;
    par.get( sKey::LineStyle(), lsstr );
    linestyle_.fromString( lsstr );

    BufferString fillstr;
    par.get( uiChartFillx::toString(uiChartFillx::Left), fillstr );
    setFillPar( fillstr, true );
    par.get( uiChartFillx::toString(uiChartFillx::Right), fillstr );
    setFillPar( fillstr, false );
}
