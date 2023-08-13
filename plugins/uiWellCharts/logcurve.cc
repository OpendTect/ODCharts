/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "logcurve.h"
#include "loggradient.h"

#include "uichartaxes.h"
#include "uichartfillx.h"
#include "uilogchart.h"

#include "draw.h"
#include "keystrs.h"
#include "mnemonics.h"
#include "multiid.h"
#include "ranges.h"
#include "separstr.h"
#include "wellchartcommon.h"
#include "welld2tmodel.h"
#include "welldata.h"
#include "welllog.h"
#include "welltrack.h"

using namespace uiWellCharts;

LogCurve::LogCurve()
    : LogData()
{}


LogCurve::LogCurve( const MultiID& wellid, const char* lognm )
    : LogData(wellid,lognm)
{
    const Well::Log* log = wd_ ? wd_->getLog( logname_ ) : nullptr;
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
    deepErase( md_ );
    deepErase( tvd_ );
    deepErase( twt_ );
    deepErase( vals_ );
    deleteAndNullPtr( scatseries_ );
    deleteAndNullPtr( leftfill_ );
    deleteAndNullPtr( rightfill_ );
    deleteAndNullPtr( axis_ );
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
    lc->copyFrom( *this );
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
	lc->scat_md_ = scat_md_;
	lc->scat_vals_ = scat_vals_;
    }

    int idx = 0;
    for ( const auto* series : series_ )
    {
	auto* ns = new uiLineSeries;
	ns->copyPoints( *series );
	lc->series_ += ns;
	lc->md_ += new TypeSet<float>( *md_[idx] );
	lc->vals_ += new TypeSet<float>( *vals_[idx] );
	idx++;
    }

    lc->setZType( ztype_, true );
    return lc;
}


void LogCurve::addTo( uiLogChart& logchart, bool show_wellnm, bool show_uom )
{
    const Well::Log* log = wd_ ? wd_->getLog( logname_ ) : nullptr;
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
	logtitle.add(" - ").add( wellName() );
    if ( show_uom )
	logtitle.add(" (").add( uomlbl_ ).add(")");

    setZType( logchart.zType(), false );
    axis_ = logchart.makeLogAxis( logtitle, min, max, reverse );
    axis_->setLineStyle( lstyle );
    disprange_ = axis_->range();
    logchart.addAxis( axis_, OD::Top );
    logchart.getZAxis()->setAxisLimits( zrange_ );
    BufferString callouttxt( wellName(), "\nDepth: %2\n", logname_ );
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


void LogCurve::addCurveFillTo( uiLogChart& logchart, bool update )
{
    if ( !lefttolog_.isEmpty() )
    {
	LogCurve* lc = logchart.getLogCurve( wellID(), lefttolog_ );
	if ( lc )
	    leftfill_->setBaseLine( lc->getSeries(), update );
    }

    if ( !righttolog_.isEmpty() )
    {
	LogCurve* lc = logchart.getLogCurve( wellID(), righttolog_ );
	if ( lc )
	    rightfill_->setBaseLine( lc->getSeries(), update );
    }
}


void LogCurve::addLog( const Well::Log& log )
{
    series_.setEmpty();
    md_.setEmpty();
    tvd_.setEmpty();
    twt_.setEmpty();
    vals_.setEmpty();
    scat_md_.setEmpty();
    scat_tvd_.setEmpty();
    scat_twt_.setEmpty();
    scat_vals_.setEmpty();
    deleteAndNullPtr( scatseries_ );
    deleteAndNullPtr( leftfill_ );
    deleteAndNullPtr( rightfill_ );
    deleteAndNullPtr( axis_ );

    auto* mds = new TypeSet<float>;
    auto* vals = new TypeSet<float>;

    for (int idx=0; idx<log.size(); idx++)
    {
	const float logval = log.value( idx );
	const float dah = dahToZ( log.dah(idx), uiWellCharts::MD );
	if ( !mIsUdf(logval) )
	{
	    mds->add( dah );
	    vals->add( logval );
	}
	else
	{
	    if ( mds->size()==1 )
	    {
		scat_md_ += mds->get( 0 );
		scat_vals_ += vals->get( 0 );
		mds->setEmpty();
		vals->setEmpty();
	    }
	    else if ( !mds->isEmpty() )
	    {
		md_ += mds;
		vals_ += vals;
		mds = new TypeSet<float>;
		vals = new TypeSet<float>;
	    }
	}
    }

    if ( !mds->isEmpty() )
    {
	md_ += mds;
	vals_ += vals;
    }
    else
    {
	delete mds;
	delete vals;
    }

    if ( !scatseries_ && !scat_md_.isEmpty() )
	scatseries_ = new uiScatterSeries;

    for ( const auto* md : md_ )
    {
	if ( !md )
	    continue;
	auto* series = new uiLineSeries;
	series_ += series;
    }

    setZType( ztype_, true );
    lefttolog_.setEmpty();
    righttolog_.setEmpty();
}


void LogCurve::removeFrom( uiLogChart& logchart )
{
    for ( auto* series : series_ )
	logchart.removeSeries( series );

    if ( scatseries_ )
	logchart.removeSeries( scatseries_ );

    deleteAndNullPtr( leftfill_ );
    deleteAndNullPtr( rightfill_ );
    lefttolog_.setEmpty();
    righttolog_.setEmpty();

    logchart.removeAxis( axis_ );
    deleteAndNullPtr( axis_ );
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


void LogCurve::setZType( ZType ztype, bool force )
{
    if ( !force && ztype==ztype_ )
	return;

    LogData::setZType( ztype, force );
    if ( !wd_ )
	return;

    const Well::Track& track = wd_->track();
    const Well::D2TModel* d2t = wd_->d2TModel();

    const bool istvd = ztype==TVD || ztype==TVDSS || ztype==TVDSD;
    const UnitOfMeasure* zduom = UnitOfMeasure::surveyDefDepthUnit();
    const UnitOfMeasure* zsuom = UnitOfMeasure::surveyDefDepthStorageUnit();
    const UnitOfMeasure* ztuom = UnitOfMeasure::surveyDefTimeUnit();

    if ( istvd && track.isEmpty() )
	return;

    if ( istvd && scatseries_ && scat_tvd_.isEmpty())
    {
	scat_tvd_.setSize( scat_md_.size() );
	track.getAllTVD( scat_md_.size(), scat_md_.arr(), zduom,
			  scat_tvd_.arr(), zduom );
    }

    if ( istvd && tvd_.isEmpty() )
    {
	for ( const auto* md : md_ )
	{
	    auto* tvd = new TypeSet<float>( md->size(), 0.f );
	    track.getAllTVD( md->size(), md->arr(), zduom, tvd->arr(), zduom );
	    tvd_ += tvd;
	}
    }

    if ( ztype==TWT && (!d2t || track.isEmpty()) )
	return;

    if ( ztype==TWT  )
    {
	if ( scatseries_ && scat_twt_.isEmpty() )
	{
	    scat_twt_.setSize( scat_md_.size() );
	    d2t->getAllTime( scat_md_.size(), scat_md_.arr(), track, zduom,
			  scat_twt_.arr(), ztuom );
	}

	if ( twt_.isEmpty() )
	{
	    for ( const auto* md : md_ )
	    {
		auto* twt = new TypeSet<float>( md->size(), 0.f );
		d2t->getAllTime( md->size(), md->arr(), track, zduom,
				 twt->arr(), ztuom );
		twt_ += twt;
	    }
	}
    }

    const float kb = getConvertedValue( track.getKbElev(), zsuom, zduom );
    const float srd = getConvertedValue( SI().seismicReferenceDatum(), zsuom,
					 zduom );
    const float zshft = ztype==TVD ? kb :
			ztype==TVDSS ? 0.f :
			ztype==TVDSD ? srd : 0.f;

    const TypeSet<float>& zs = istvd ? scat_tvd_ :
			       ztype==TWT ? scat_twt_ : scat_md_;

    if ( scatseries_ && !scatseries_->replace_y(zs.size(),zs.arr(),zshft) )
	scatseries_->replace( zs.size(), scat_vals_.arr(), zs.arr(), 0.f,
			      zshft );

    const ObjectSet<TypeSet<float>>& z = istvd ? tvd_ :
					 ztype==TWT ? twt_ : md_;

    for ( int idx=0; idx<series_.size(); idx++ )
    {
	if ( !series_[idx]->replace_y(z[idx]->size(), z[idx]->arr(),zshft) )
	    series_[idx]->replace( z[idx]->size(), vals_[idx]->arr(),
				   z[idx]->arr(), 0.f, zshft );
    }

    if ( leftfill_ && leftfill_->gradientImg() )
    {
	mDynamicCastGet(LogGradient*,lg,leftfill_->gradientImg())
	lg->setZType( ztype, false );
    }
    if ( rightfill_ && rightfill_->gradientImg() )
    {
	mDynamicCastGet(LogGradient*,lg,rightfill_->gradientImg())
	lg->setZType( ztype, false );
    }
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
    const uiChartFillx::FillType filltyp =
		fill ? fill->fillType() : uiChartFillx::NoFill;
    FileMultiString fms = uiChartFillx::toString( filltyp );
    const float baseval = fill ? fill->baseLineValue() : mUdf(float);
    const bool hascurve = fill ? fill->hasBaseLineSeries() : false;
    const uiWellCharts::FillLimit flim =
		hascurve ? uiWellCharts::Curve
			 : (mIsUdf(baseval) ? uiWellCharts::Track
					    : uiWellCharts::Baseline);
    fms += toString( flim );

    if ( flim==uiWellCharts::Baseline )
	fms += baseval;
    else if ( flim==uiWellCharts::Curve )
	fms += tolognm;

    if ( fill )
    {
	if ( fill->fillType()==uiChartFillx::ColorFill )
	{
	    BufferString col;
	    fill->color().fill( col );
	    fms += FileMultiString( col );
	}
	else if ( fill->fillType()==uiChartFillx::GradientFill )
	    fms += fill->gradientImg()->toString();
    }

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
    if ( left )
	lefttolog_.setEmpty();
    else
	righttolog_.setEmpty();

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
	LogGradient* lg = new LogGradient( wellID() );
	lg->fromString( gradfms );
	fill->setGradientImg( lg, false );
    }
}


void LogCurve::usePar( const IOPar& par, bool styleonly )
{
    LogData::usePar( par, styleonly );
    if ( !styleonly )
    {
	const Well::Log* log = wd_ ? wd_->getLog( logname_ ) : nullptr;
	if ( log )
	    addLog( *log );
    }

    BufferString lsstr;
    par.get( sKey::LineStyle(), lsstr );
    OD::LineStyle ls;
    ls.fromString( lsstr );
    setLineStyle( ls );

    BufferString fillstr;
    par.get( uiChartFillx::toString(uiChartFillx::Left), fillstr );
    setFillPar( fillstr, true );
    par.get( uiChartFillx::toString(uiChartFillx::Right), fillstr );
    setFillPar( fillstr, false );
}
