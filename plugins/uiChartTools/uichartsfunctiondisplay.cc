/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uichartsfunctiondisplay.h"

#include "chartutils.h"
#include "scaler.h"

#include "uichart.h"
#include "uichartaxes.h"
#include "uichartseries.h"
#include "uigeom.h"


// uiChartsFunctionDisplay
uiChartsFunctionDisplay::uiChartsFunctionDisplay( uiParent* p, const Setup& su )
    : uiFuncDispBase(su)
    , uiChartView(p,"Function Display")
    , pointSelected(this)
    , pointChanged(this)
{
    setPrefWidth( setup_.canvaswidth_ );
    setPrefHeight( setup_.canvasheight_ );
    setStretch( 2, 2 );

    auto* chart = new uiChart;
    chart->displayLegend( setup_.showlegend_ );
    chart->setTitleBold( true );
    setChart( chart );
    setChartStyle();

    uiChartsAxisHandler::Setup asu( uiRect::Bottom, setup_.canvaswidth_,
			         setup_.canvasheight_ );
    asu.noaxisline( setup_.noxaxis_ );
    asu.noaxisannot( asu.noaxisline_ ? true : !setup_.annotx_ );
    asu.nogridline( asu.noaxisline_ ? true : setup_.noxgridline_ );
    asu.border_ = setup_.border_;
    asu.annotinint_ = setup_.xannotinint_;
    xax_ = new uiChartsAxisHandler( this, asu );

    asu.noaxisline( setup_.noyaxis_ );
    asu.noaxisannot( asu.noaxisline_ ? true : !setup_.annoty_ );
    asu.nogridline( asu.noaxisline_ ? true : setup_.noygridline_ );
    asu.side( uiRect::Left );
    asu.annotinint_ = setup_.yannotinint_;
    xax_->setBounds(setup_.xrg_);
    yax_ = new uiChartsAxisHandler( this, asu );
    asu.noaxisline( setup_.noy2axis_ );
    asu.noaxisannot( asu.noaxisline_ ? true : !setup_.annoty2_ );
    asu.nogridline( setup_.noy2gridline_ ).gridlinestyle( setup_.y2linestyle_ );
    asu.side( uiRect::Right );
    y2ax_ = new uiChartsAxisHandler( this, asu );

    if ( setup_.editable_ )
    {
	mAttachCB(mousePress,uiChartsFunctionDisplay::mousePressCB);
	mAttachCB(mouseRelease,uiChartsFunctionDisplay::mouseReleaseCB);
	mAttachCB(mouseMove,uiChartsFunctionDisplay::mouseMoveCB);
	mAttachCB(pointSelected,uiChartsFunctionDisplay::pointSelectedCB);
	setup_.ptsnaptol_ = 1.0;
    }
}


uiChartsFunctionDisplay::~uiChartsFunctionDisplay()
{
    detachAllNotifiers();
    cleanUp();
    delete xax_; delete yax_; delete y2ax_;
}


void uiChartsFunctionDisplay::cleanUp()
{

}


void uiChartsFunctionDisplay::setTitle( const uiString& uistr )
{
    chart()->setTitle( uistr );
}


uiChartSeries* uiChartsFunctionDisplay::getSeries( bool y2 )
{
    if ( y2 && y2series_ )	return y2series_;
    if ( y2 && y2area_ )	return y2area_;
    if ( yseries_ )		return yseries_;
    if ( yarea_ )		return yarea_;
    return nullptr;
}


Geom::PointF uiChartsFunctionDisplay::mapToPosition( const Geom::PointF& pt,
						     bool y2 )
{
    uiChartSeries* series = getSeries( y2 );
    if (!series )
	return 	Geom::PointF::udf();

    return chart()->mapToPosition( pt, series );
}


Geom::PointF uiChartsFunctionDisplay::mapToValue( const Geom::PointF& pt,
						  bool y2 )
{
    uiChartSeries* series = getSeries( y2 );
    if (!series )
	return Geom::PointF::udf();

    return chart()->mapToValue( pt, series );
}


void uiChartsFunctionDisplay::draw()
{
    makeSeries();
}


void uiChartsFunctionDisplay::drawMarkLines()
{}


bool uiChartsFunctionDisplay::setSelPt( const Geom::PointF& pos )
{
    int newsel = -1;
    float mindistsq = 1e30;
    for ( int idx=0; idx<xvals_.size(); idx++ )
    {
	const Geom::PointF pospt = mapToPosition( Geom::PointF(xvals_[idx],
							       yvals_[idx]) );
	const float distsq = (pospt.x_-pos.x_)*(pospt.x_-pos.x_) +
			     (pospt.y_-pos.y_)*(pospt.y_-pos.y_);
	if ( distsq < mindistsq )
	{
	    newsel = idx;
	    mindistsq = distsq;
	}
    }

    selpt_ = -1;
    if ( selseries_ )
    {
	const float tol = selseries_->markerSize() * setup_.ptsnaptol_;
	if ( mindistsq > tol*tol )
	    return false;
    }
    if ( !xvals_.validIdx(newsel) )
	return false;

    selpt_ = newsel;
    return true;
}


void uiChartsFunctionDisplay::mousePressCB( CallBacker* cb )
{
    if ( mousedown_ ) return;

    mousedown_ = true;
    mCBCapsuleUnpack(const Geom::PointF&,pos,cb);
    const bool isctrl = keyModifier()==OD::ControlButton;
    const bool isnorm = keyModifier()==OD::NoButton;
    const bool isselected = setSelPt( pos );

    if ( !isselected && isnorm )
    {
	addPoint( pos );
	pointSelectedCB( nullptr );
	pointSelected.trigger();
	draw();
	return;
    }
    pointSelectedCB( nullptr );
    pointSelected.trigger();

    if ( !xvals_.validIdx(selpt_) )
	return;

    if ( isselected && isctrl && xvals_.size()>2 )
    {
	xvals_.removeSingle( selpt_ );
	yvals_.removeSingle( selpt_ );
	selpt_ = -1;
	pointSelectedCB( nullptr );
	pointChanged.trigger();
	draw();
    }
}


void uiChartsFunctionDisplay::mouseReleaseCB( CallBacker* )
{
    if ( !mousedown_ )
	return;

    mousedown_ = false;
}


void uiChartsFunctionDisplay::mouseMoveCB( CallBacker* cb )
{
    if ( !mousedown_ )
	return;

    mCBCapsuleUnpack(const Geom::PointF&,pos,cb);
    const bool isnorm = keyModifier()==OD::NoButton;
    if ( !isnorm || !xvals_.validIdx(selpt_) )
	return;

    Geom::PointF newpos = mapToValue( pos );
    if ( selpt_>0 && xvals_[selpt_-1]>=newpos.x_ )
        newpos.x_ = xvals_[selpt_-1];
    else if ( selpt_<xvals_.size()-1 && xvals_[selpt_+1]<=newpos.x_ )
        newpos.x_ = xvals_[selpt_+1];

    newpos.x_ = xAxis()->range().limitValue( newpos.x_ );
    newpos.y_ = yAxis( false )->range().limitValue( newpos.y_ );
    xvals_[selpt_] = newpos.x_;
    yvals_[selpt_] = newpos.y_;

    pointSelectedCB( nullptr );
    pointChanged.trigger();
    draw();
}


void uiChartsFunctionDisplay::pointSelectedCB( CallBacker* )
{
    if ( !selseries_ )
    {
	selseries_ = new uiScatterSeries;
	chart()->addSeries( selseries_ );
	selseries_->attachAxis( xAxis()->axis() );
	selseries_->attachAxis( yAxis(false)->axis() );
	selseries_->setColor( OD::Color(255,0,255,200) );
    }
    selseries_->clear();
    if ( xvals_.validIdx( selpt_ ) )
	selseries_->append( xvals_[selpt_], yvals_[selpt_] );
}


uiChartsAxisHandler* uiChartsFunctionDisplay::xAxis() const
{
    return dCast(uiChartsAxisHandler*,xax_);
}


uiChartsAxisHandler* uiChartsFunctionDisplay::yAxis( bool y2 ) const
{
    return y2 ? dCast(uiChartsAxisHandler*,y2ax_)
	      : dCast(uiChartsAxisHandler*,yax_);
}


void uiChartsFunctionDisplay::makeSeries()
{
    if ( !yseries_ && !setup().fillbelow_ )
    {
	yseries_ = new uiLineSeries;
	yseries_->setName( setup().yseriesname_ );
	chart()->addSeries( yseries_ );
	yseries_->attachAxis( xAxis()->axis() );
	yseries_->attachAxis( yAxis(false)->axis() );
	if ( setup().editable_ )
	{
	    //TODO?
	}
    }

    if ( !yarea_ && setup().fillbelow_ )
    {
	yarea_ = new uiAreaSeries( new uiLineSeries );
	yarea_->setName( setup().yseriesname_ );
	chart()->addSeries( yarea_ );
	yarea_->attachAxis( xAxis()->axis() );
	yarea_->attachAxis( yAxis(false)->axis() );
    }

    if ( !y2series_ && !setup().fillbelowy2_ )
    {
	y2series_ = new uiLineSeries;
	y2series_->setName( setup().y2seriesname_ );
	chart()->addSeries( y2series_ );
	y2series_->attachAxis( xAxis()->axis() );
	y2series_->attachAxis( yAxis(!setup().useyscalefory2_)->axis() );
    }

    if ( !y2area_ && setup().fillbelowy2_ )
    {
	y2area_ = new uiAreaSeries( new uiLineSeries );
	y2area_->setName( setup().y2seriesname_ );
	chart()->addSeries( y2area_ );
	y2area_->attachAxis( xAxis()->axis() );
	y2area_->attachAxis( yAxis(!setup().useyscalefory2_)->axis() );
    }

    if ( yseries_ && xVals().arr() && yVals().arr() )
	yseries_->replace( size(), xVals().arr(), yVals().arr() );

    if ( y2series_ && y2xVals().arr() && y2yVals().arr() )
	y2series_->replace( y2size(), y2xVals().arr(), y2yVals().arr() );

    if ( yarea_ && xVals().arr() && yVals().arr() )
	yarea_->upperSeries()->replace( size(), xVals().arr(), yVals().arr() );

    if ( y2area_ && y2xVals().arr() && y2yVals().arr() )
	y2area_->upperSeries()->replace( y2size(), y2xVals().arr(),
					 y2yVals().arr() );

    setSeriesStyle();
}


void uiChartsFunctionDisplay::setSeriesStyle()
{
    if ( yseries_ )
    {
	yseries_->setVisible( !setup().fillbelow_ );
	yseries_->setLineStyle( setup().ylinestyle_ );
	yseries_->setLineVisible( setup().drawliney_ );
	yseries_->setPointsVisible( setup().drawscattery1_ );
    }

    if ( y2series_ )
    {
	y2series_->setVisible( !setup().fillbelowy2_ );
	y2series_->setLineStyle( setup().y2linestyle_ );
	y2series_->setVisible( setup().drawliney2_ );
	y2series_->setPointsVisible( setup().drawscattery2_ );
    }

    if ( yarea_ )
    {
	yarea_->setVisible( setup().fillbelow_ );
	yarea_->setBorderStyle( setup().ylinestyle_ );
	yarea_->setColor( setup().ylinestyle_.color_ );
	yarea_->setLinesVisible( setup().drawliney_ );
	yarea_->setPointsVisible( setup().drawscattery1_ );
    }

    if ( y2area_ )
    {
	y2area_->setVisible( setup().fillbelowy2_ );
	y2area_->setBorderStyle( setup().y2linestyle_ );
	y2area_->setColor( setup().y2linestyle_.color_ );
	y2area_->setLinesVisible( setup().drawliney2_ );
	y2area_->setPointsVisible( setup().drawscattery2_ );
    }
}


void uiChartsFunctionDisplay::setChartStyle()
{
    chart()->setBackgroundColor( setup().bgcol_ );
    uiBorder b = setup().border_;
    chart()->setMargins( b.left(), b.top(), b.right(), b.bottom() );
}



// uiChartsMultiFunctionDisplay
uiChartsMultiFunctionDisplay::uiChartsMultiFunctionDisplay( uiParent* p,
							    const Setup& su )
    : uiMultiFuncDispBase(su)
    , uiChartView(p,"Multi-Function Display")
{
    setPrefWidth( setup_.canvaswidth_ );
    setPrefHeight( setup_.canvasheight_ );
    setStretch( 2, 2 );

    auto* chart = new uiChart;
    chart->displayLegend( setup_.showlegend_ );
    chart->setTitleBold( true );
    setChart( chart );
    setChartStyle();

    uiChartsAxisHandler::Setup asu( uiRect::Bottom, setup_.canvaswidth_,
			         setup_.canvasheight_ );
    asu.noaxisline( setup_.noxaxis_ );
    asu.noaxisannot( asu.noaxisline_ ? true : !setup_.annotx_ );
    asu.nogridline( asu.noaxisline_ ? true : setup_.noxgridline_ );
    asu.border_ = setup_.border_;
    asu.annotinint_ = setup_.xannotinint_;
    xax_ = new uiChartsAxisHandler( this, asu );

    asu.noaxisline( setup_.noyaxis_ );
    asu.noaxisannot( asu.noaxisline_ ? true : !setup_.annoty_ );
    asu.nogridline( asu.noaxisline_ ? true : setup_.noygridline_ );
    asu.side( uiRect::Left );
    asu.annotinint_ = setup_.yannotinint_;
    xax_->setBounds(setup_.xrg_);
    yax_ = new uiChartsAxisHandler( this, asu );
}


uiChartsMultiFunctionDisplay::~uiChartsMultiFunctionDisplay()
{
    detachAllNotifiers();
    delete xax_;
    delete yax_;
}


void uiChartsMultiFunctionDisplay::setEmpty()
{
    chart()->removeAllSeries();
    series_.setEmpty();
    uiMultiFuncDispBase::setEmpty();
}


void uiChartsMultiFunctionDisplay::setTitle( const uiString& uistr )
{
    chart()->setTitle( uistr );
}


uiXYChartSeries* uiChartsMultiFunctionDisplay::getSeries( int idx )
{
    return series_.validIdx(idx) ? series_.get( idx ) : nullptr;
}


Geom::PointF uiChartsMultiFunctionDisplay::mapToPosition(
						const Geom::PointF& pt )
{
    return chart()->mapToPosition( pt, nullptr );
}


Geom::PointF uiChartsMultiFunctionDisplay::mapToValue( const Geom::PointF& pt )
{
    return chart()->mapToValue( pt, nullptr );
}


void uiChartsMultiFunctionDisplay::setVisible( const char* nm, bool yn )
{
    const int index = indexOf( nm );
    if ( series_.validIdx(index) )
    {
	functions_[index]->isvisible_ = yn;
	series_[index]->setVisible( yn );
    }
}


void uiChartsMultiFunctionDisplay::draw()
{
    if ( functions_.size() != series_.size() )
	return;

    for ( int idx=0; idx<functions_.size(); idx++ )
    {
	const auto& func = *functions_.get( idx );
	mDynamicCastGet(uiXYChartSeries*,series,getSeries(idx))
	if ( !series )
	    continue;

	series->setVisible( func.isvisible_ );
	if ( func.isvisible_ && !func.xvals_.isEmpty() &&
				!func.yvals_.isEmpty() )
	    series->replace( func.xvals_.size(), func.xvals_.arr(),
		    	     func.yvals_.arr() );
    }
}


uiChartsAxisHandler* uiChartsMultiFunctionDisplay::xAxis() const
{
    return dCast(uiChartsAxisHandler*,xax_);
}


uiChartsAxisHandler* uiChartsMultiFunctionDisplay::yAxis() const
{
    return dCast(uiChartsAxisHandler*,yax_);
}


void uiChartsMultiFunctionDisplay::addFunction( FunctionPlotData* func )
{
    uiMultiFuncDispBase::addFunction( func );
    makeSeries( *func );
    gatherInfo();
    draw();
}


void uiChartsMultiFunctionDisplay::removeFunction( const char* nm )
{
    const int index = indexOf( nm );
    if ( index < 0 )
	return;

    chart()->removeSeries( series_.removeSingle(index) );
    uiMultiFuncDispBase::removeFunction( nm );
    gatherInfo();
    draw();
}


void uiChartsMultiFunctionDisplay::makeSeries( const FunctionPlotData& func )
{
    uiXYChartSeries* series = nullptr;
    if ( func.type_ == FunctionPlotData::Line )
    {
	auto* lineseries = new uiLineSeries;
	lineseries->setLineStyle( func.linestyle_ );
	series = lineseries;
    }
    else
    {
	auto* scatterseries = new uiScatterSeries;
	MarkerStyle2D::Type shape = func.markerstyle_.type_;
	//TODO: Support other shapes
	scatterseries->setShape( shape==MarkerStyle2D::Square ?
			uiScatterSeries::Square : uiScatterSeries::Circle );
	scatterseries->setColor( func.markerstyle_.color_ );
	scatterseries->setBorderColor( func.markerstyle_.color_ );
	scatterseries->setMarkerSize( func.markerstyle_.size_ );
	series = scatterseries;
    }

    series->setName( func.name() );
    if ( setup().showcallout_ )
	series->setCalloutTxt( func.callouttext_.isEmpty() ?
				func.callouttext_.buf() : func.name().buf() );

    chart()->addSeries( series );
    series->attachAxis( xAxis()->axis() );
    series->attachAxis( yAxis()->axis() );
    series_.add( series );
}


void uiChartsMultiFunctionDisplay::setChartStyle()
{
    chart()->setBackgroundColor( setup().bgcol_ );
    uiBorder b = setup().border_;
    chart()->setMargins( b.left(), b.top(), b.right(), b.bottom() );
}


// uiChartsAxisHandler
uiChartsAxisHandler::uiChartsAxisHandler( uiChartView* cv, const Setup& su )
    : uiAxisHandlerBase(su)
    , chartview_(cv)
{
    makeAxis();
}


uiChartsAxisHandler::~uiChartsAxisHandler()
{
    deleteAndNullPtr( axis_ );
}


void uiChartsAxisHandler::setCaption( const uiString& uistr )
{
    if ( axis_ )
	axis_->setTitle( uistr );
}


uiString uiChartsAxisHandler::getCaption() const
{
    uiString uistr;
    if ( axis_ )
	uistr = axis_->title();

    return uistr;
}


void uiChartsAxisHandler::setBorder( const uiBorder& b )
{
    setup().border_ = b;
}


void uiChartsAxisHandler::setIsLog( bool yn )
{
    if ( setup().islog_==yn )
	return;

    makeAxis();
}


void uiChartsAxisHandler::setRange( const StepInterval<float>& rg,
				    float* astart )
{
    if ( !axis_ )
	return;

    axis_->setDynamicTicks( rg.step_, astart ? *astart : rg.start_ );
    axis_->setRange( rg );
}


void uiChartsAxisHandler::setBounds( Interval<float> rg )
{
    StepInterval<float> steprg;
    if ( rg.isUdf() )
	steprg.set( 0.f, 1.f, 1.f);
    else
    {
	const bool isrev = rg.isRev();
	steprg = StepInterval<float>( rg ).niceInterval( 5, isrev );
    }

    setRange( steprg );
}


StepInterval<float> uiChartsAxisHandler::range() const
{
    StepInterval<float> res = StepInterval<float>::udf();
    if ( axis_ )
	res.set( axis_->range(), axis_->getTickInterval() );

    return res;
}


void uiChartsAxisHandler::makeAxis()
{
    auto* chart = chartview_->chart();
    if ( !chart )
	return;

    if ( axis_ )
    {
	chart->removeAxis( axis_ );
	deleteAndNullPtr( axis_ );
    }

    if ( setup().islog_ )
    {
	auto* axis = new uiLogValueAxis;
	axis->setBase( 10 );
	axis->setLabelFormat( "%g" );
	axis_ = axis;
    }
    else
    {
	auto* axis = new uiValueAxis;
	axis->setLabelFormat( "%g" );
	axis_ = axis;
    }

    setAxisStyle();

    chart->addAxis( axis_, fromSide(setup().side_) );
}


void uiChartsAxisHandler::setAxisStyle()
{
    if ( !axis_ )
	return;

    const OD::Color nmcolor = setup().nmcolor_ == OD::Color::NoColor() ?
				    setup().style_.color_ : setup().nmcolor_;
    axis_->setLineVisible( !setup().noaxisline_ );
    axis_->setLabelsVisible( !setup().noaxisannot_ );
    axis_->setGridLineVisible( !setup().nogridline_ );
    axis_->setMinorGridLineVisible( false );
    axis_->setTitleColor( nmcolor );
    axis_->setLineStyle( setup().style_ );
    axis_->setGridStyle( setup().gridlinestyle_ );
}



// uiChartsFunctionDrawer
uiChartsFunctionDrawer::uiChartsFunctionDrawer( uiParent* p, const Setup& su )
    : uiFuncDrawerBase(su)
    , uiChartView(p, "Function Drawer")
{
    setMinimumWidth( su.canvaswidth_ );
    setMinimumHeight( su.canvasheight_ );
    setStretch( 2, 2 );

    auto* chart = new uiChart;
    chart->displayLegend( false );
    setChart( chart );

    uiChartsAxisHandler::Setup asu( uiRect::Bottom );
    asu.maxnrchars_ = 8;
    asu.border_ = uiBorder(20,20,20,20);

    auto* xax = new uiChartsAxisHandler( this, asu );
    xax->setRange( su.xaxrg_ );

    asu.side( uiRect::Left ); asu.islog_ = false;
    auto* yax = new uiChartsAxisHandler( this, asu );
    yax->setRange( su.yaxrg_ );

    xax->setCaption( su.xaxcaption_ ); yax->setCaption( su.yaxcaption_ );
    xax_ = xax; 			yax_ = yax;
}


uiChartsFunctionDrawer::~uiChartsFunctionDrawer()
{
    delete xax_;
    delete yax_;
}


void uiChartsFunctionDrawer::draw( CallBacker* )
{
    chart()->removeAllSeries();

    if ( !selitemsidx_.size() && functions_.size() )
	selitemsidx_ += 0;

    LinScaler scaler( funcrg_.start_, xax_->range().start_,
                      funcrg_.stop_, xax_->range().stop_ );
    for ( int ifun=0; ifun<selitemsidx_.size(); ifun++ )
    {
	const int selidx = selitemsidx_[ifun];
	DrawFunction* func =
		functions_.validIdx(selidx) ? functions_[selidx] : 0;
	if ( !func )
	    return;

	auto* series = new uiLineSeries;
	chart()->addSeries( series );
	series->attachAxis( xAxis()->axis() );
	series->attachAxis( yAxis()->axis() );
	OD::LineStyle ls;
	ls.width_ = setup().width_;
	ls.color_ = func->color_;
	series->setLineStyle( ls );

	StepInterval<float> xrg( funcrg_ );
        xrg.step_ = 0.0001;
	TypeSet<float> xvals;
	TypeSet<float> yvals;
	for ( int idx=0; idx<xrg.nrSteps()+1; idx++ )
	{
	    float x = xrg.atIndex( idx );
	    const float y = func->mathfunc_->getValue( x );
	    x = (float) ( scaler.scale( x ) );
	    xvals += x; yvals += y;
	}
	series->replace( xvals.size(), xvals.arr(), yvals.arr() );
    }
}


uiChartsAxisHandler* uiChartsFunctionDrawer::xAxis() const
{
    return dCast(uiChartsAxisHandler*,xax_);
}


uiChartsAxisHandler* uiChartsFunctionDrawer::yAxis() const
{
    return dCast(uiChartsAxisHandler*,xax_);
}
