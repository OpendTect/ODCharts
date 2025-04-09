/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uilogviewtable.h"

#include "logcurve.h"
#include "paralleltask.h"
#include "uichartaxes.h"
#include "uilogchart.h"
#include "uilogview.h"
#include "uilogviewtoolgrp.h"
#include "uimsg.h"
#include "uitable.h"
#include "wellman.h"


uiLogViewTable::uiLogViewTable( uiParent* p, int nrcol, bool showtools )
    : uiGroup(p)
    , chartSelectionChg(this)
    , showtools_(showtools)
{
    const int nrrows = showtools_ ? 2 : 1;
    logviews_ = new uiTable( this, uiTable::Setup(nrrows,0)
				.rowgrow(false).colgrow(true)
				.fillrow(true).fillcol(true)
				.defrowlbl(false).defcollbl(false)
				.insertrowallowed(false).removerowallowed(false)
				.insertcolallowed(false).removecolallowed(false)
				.mincolwdt(30.0), "" );
    logviews_->setStretch( 2, 2 );
    logviews_->showGrid( false );
    logviews_->setTopHeaderHidden( false );
    logviews_->setLeftHeaderHidden( true );
    logviews_->setBackgroundColor( this->backgroundColor() );
    logviews_->showOuterFrame( false );
    logviews_->setRowResizeMode( uiTable::ResizeToContents );
    logviews_->setRowStretchable( showtools_ ? 1 : 0, true );
    logviews_->setColumnResizeMode( uiTable::Stretch );
    logviews_->setSelectionMode( uiTable::SingleColumn );

    setNumViews( nrcol );

    mAttachCB( logviews_->colInserted, uiLogViewTable::trackaddCB );
    mAttachCB( logviews_->colDeleted, uiLogViewTable::trackremoveCB );
    mAttachCB( logviews_->columnClicked, uiLogViewTable::colSelectCB );
    mAttachCB( logviews_->leftClicked, uiLogViewTable::selectCB );
}


uiLogViewTable::~uiLogViewTable()
{
    detachAllNotifiers();
    setEmpty();
}


int uiLogViewTable::currentView() const
{
    return logviews_->currentCol();
}


void uiLogViewTable::setCurrentView( int col )
{
    if ( !validIdx(col) )
	return;

    const int row = showtools_ ? 1 : 0;
    logviews_->setCurrentCell( RowCol(row,col) );
    selectCB( nullptr );
}


bool uiLogViewTable::validIdx( int col ) const
{
    return col>=0 && col<size();
}


int uiLogViewTable::size() const
{
    return logviews_->nrCols();
}


void uiLogViewTable::setEmpty()
{
    primarychart_ = nullptr;
    primaryzrange_.setUdf();
    selected_ = -1;
    for ( int col=size()-1; col>=0; col-- )
    {
	for ( int row=0; row<logviews_->nrRows(); row++ )
	    logviews_->clearCellObject( RowCol(row,col) );

	logviews_->removeColumn( col );
    }
}


bool uiLogViewTable::isEmpty() const
{
    return size()==0;
}


void uiLogViewTable::setNumViews( int nvw )
{
    setEmpty();
    logviews_->setNrCols( nvw );
    for ( int col=0; col<nvw; col++ )
	addViewer( col );

    setCurrentView( 0 );
    selectCB( nullptr );
}


mDefParallelCalc3Pars(SingleLogLoader,
		  od_static_tr("SingleLogLoader", "Single log loader"),
		      const BufferStringSet&, wellnms,
		      const ObjectSet<const Well::Log>&, logs,
		      ObjectSet<LogCurve>&, logcurves)
mDefParallelCalcBody( ,
    const BufferString& wellnm = wellnms_.get( idx );
    const Well::Log& log = *logs_[idx];
    delete logcurves_.replace( idx, new LogCurve(wellnm,log) );
    ,
)


void uiLogViewTable::addWellData( const BufferStringSet& wellnms,
				  const ObjectSet<const Well::Log>& logs,
				  const OD::LineStyle& ls,
				  const char* suffix )
{
    ObjectSet<LogCurve> logcurves;
    for ( int idx=0; idx<wellnms.size(); idx++ )
	logcurves += new LogCurve;

    SingleLogLoader loader( wellnms.size(), wellnms, logs, logcurves );
    loader.execute();

    for ( int vwidx=0; vwidx<wellnms.size(); vwidx++ )
    {
        uiLogChart* chart = getLogChart( vwidx );
	if ( !chart )
	    continue;

	LogCurve* curvecopy = logcurves[vwidx]->clone();
	if ( suffix )
	{
	    LogCurve* logcurve_in = chart->getLogCurve( curvecopy->logName() );
	    BufferString lognm( curvecopy->logName(), suffix );
	    curvecopy->setLogName( lognm );
	    chart->removeLogCurve( lognm );
	    if ( logcurve_in )
	    {
		const Interval<float>& dr = logcurve_in->dispRange();
		if ( logcurve_in->mnemonic()==curvecopy->mnemonic() )
		    curvecopy->setDisplayRange( dr );
		else
		    curvecopy->setDisplayRange( StepInterval<float>::udf() );
	    }
	}

	chart->addLogCurve( curvecopy, ls, false, true );
	updateViewLabel( vwidx );
    }

    updatePrimaryZrangeCB( nullptr );
    deepErase( logcurves );
}


mDefParallelCalc3Pars(MultiLogLoader,
		  od_static_tr("MultiLogLoader", "Multi log loader"),
		      const DBKeySet&, wellids,
		      const ManagedObjectSet<TypeSet<int>>&, logidxs,
		      ManagedObjectSet<ObjectSet<LogCurve>>&, logcurves)
mDefParallelCalcBody( ,
    const DBKey& wkey = wellids_[idx];
    const TypeSet<int>& logidx = *logidxs_[idx];
    BufferStringSet lognms;
    Well::Man::getLogNamesByID( wkey, lognms );
    for ( const auto& lidx : logidx )
    {
	if ( !lognms.validIdx(lidx) )
	    continue;
	logcurves_[idx]->add( new LogCurve(wkey, lognms.get(lidx)) );
    }
    ,
)


void uiLogViewTable::addWellData( const DBKeySet& wellids,
				const ManagedObjectSet<TypeSet<int>>& logidxs )
{
    setNumViews( wellids.size() );
    ManagedObjectSet<ObjectSet<LogCurve>> logcurves;
    for ( int idx=0; idx<wellids.size(); idx++ )
	logcurves += new ObjectSet<LogCurve>();

    uiUserShowWait uisw( this, tr("Loading") );
    MultiLogLoader loader( wellids.size(), wellids, logidxs, logcurves );
    loader.execute();

    for ( int vwidx=0; vwidx<logcurves.size(); vwidx++ )
    {
	if ( logcurves[vwidx]->isEmpty() )
	    continue;

	uiLogChart* chart = getLogChart( vwidx );
	if ( !chart )
	    continue;

	ObjectSet<LogCurve> curvecopy;
	for ( const auto* curve : *logcurves[vwidx] )
	    curvecopy += curve->clone();

	chart->addLogCurves( curvecopy, false, true );
	updateViewLabel( vwidx );
	deepErase( *logcurves[vwidx] );
	uisw.setMessage(
		tr("Display: %1 %").arg(int((vwidx+1)/logcurves.size()*100.f)) );

    }

    updatePrimaryZrangeCB( nullptr );
}


void uiLogViewTable::addWellData( const DBKeySet& wellids,
				  const ManagedObjectSet<TypeSet<int>>& logidxs,
				  const BufferStringSet& mrknms )
{
    addWellData( wellids, logidxs );
    for ( int vwidx=0; vwidx<size(); vwidx++ )
    {
	uiLogChart* chart = getLogChart( vwidx );
	if ( !chart )
	    continue;

	for ( const auto* mrknm : mrknms )
	    chart->addMarker( wellids[vwidx], *mrknm, false );
    }
}


mDefParallelCalc3Pars(MultiLogLoader2,
		  od_static_tr("MultiLogLoader2", "Multi log loader 2"),
		      const DBKeySet&, wellids,
		      const BufferStringSet&, lognms,
		      ManagedObjectSet<ObjectSet<LogCurve>>&, logcurves)
mDefParallelCalcBody( ,
    const DBKey& wkey = wellids_[idx];
    for ( const auto* lognm : lognms_ )
	logcurves_[idx]->add( new LogCurve(wkey, *lognm) );
    ,
)


void uiLogViewTable::addWellData( const DBKeySet& wellids,
				  const BufferStringSet& lognms,
				  const BufferStringSet& mrknms )
{
    setNumViews( wellids.size() );
    ManagedObjectSet<ObjectSet<LogCurve>> logcurves;
    for ( int idx=0; idx<wellids.size(); idx++ )
	logcurves += new ObjectSet<LogCurve>();

    uiUserShowWait uisw( this, tr("Loading") );
    MultiLogLoader2 loader( wellids.size(), wellids, lognms, logcurves );
    loader.execute();

    for ( int vwidx=0; vwidx<logcurves.size(); vwidx++ )
    {
	if ( logcurves[vwidx]->isEmpty() )
	    continue;

	uiLogChart* chart = getLogChart( vwidx );
	if ( !chart )
	    continue;

	ObjectSet<LogCurve> curvecopy;
	for ( const auto* curve : *logcurves[vwidx] )
	    curvecopy += curve->clone();

	chart->addLogCurves( curvecopy, false, true );
	updateViewLabel( vwidx );
	deepErase( *logcurves[vwidx] );
	for ( const auto* mrknm : mrknms )
	    chart->addMarker( wellids[vwidx], *mrknm, false );

	uisw.setMessage(
		tr("Display: %1%").arg(int((vwidx+1)/logcurves.size()*100.f)) );
    }


    updatePrimaryZrangeCB( nullptr );
}


mDefParallelCalc3Pars(SingleLogLoader2,
		  od_static_tr("SingleLogLoader2", "Single log loader 2"),
		      const DBKeySet&, wellids,
		      const BufferStringSet, lognms,
		      ObjectSet<LogCurve>&, logcurves)
mDefParallelCalcBody( ,
    const DBKey& wkey = wellids_[idx];
    delete logcurves_.replace( idx, new LogCurve(wkey, lognms_.get(idx)) );
    ,
)


void uiLogViewTable::addWellData( const DBKeySet& wellids,
				  const BufferStringSet& lognms,
				  const OD::LineStyle& ls )
{
    ObjectSet<LogCurve> logcurves;
    for ( int idx=0; idx<wellids.size(); idx++ )
	logcurves += new LogCurve;

    SingleLogLoader2 loader( wellids.size(), wellids, lognms, logcurves );
    loader.execute();

    for ( int vwidx=0; vwidx<wellids.size(); vwidx++ )
    {
        uiLogChart* chart = getLogChart( vwidx );
	if ( !chart )
	    continue;

	LogCurve* curvecopy = logcurves[vwidx]->clone();
	chart->addLogCurve( curvecopy, ls, false, true );
	updateViewLabel( vwidx );
    }

    updatePrimaryZrangeCB( nullptr );
    deepErase( logcurves );
}


uiLogChart* uiLogViewTable::getCurrentLogChart()
{
    return selected_==-1 ? nullptr : getLogChart( selected_ );
}


uiLogChart* uiLogViewTable::getLogChart( int col )
{
    uiLogView* logview = getLogView( col );
    return logview ?  logview->logChart() : nullptr;
}


uiLogView* uiLogViewTable::getCurrentLogView()
{
    return selected_==-1 ? nullptr : getLogView( selected_ );
}


uiLogView* uiLogViewTable::getLogView( int col )
{
    if ( !validIdx(col) )
	return nullptr;

    const int row = showtools_ ? 1 : 0;
    const RowCol curcell( row, col );
    mDynamicCastGet(uiLogView*,logview,logviews_->getCellObject(curcell))
    return logview;
}


Interval<float> uiLogViewTable::primaryZRange() const
{
    return primaryzrange_;
}


void uiLogViewTable::setPrimaryZRange( const Interval<float> range, bool apply )
{
    if ( !apply && primaryzrange_.includes(range) )
	return;

    primaryzrange_ = range;
    for ( int idx=0; idx<size(); idx++ )
    {
	uiLogChart* lchart = getLogChart( idx );
	if ( lchart )
	    lchart->getZAxis()->setAxisLimits( primaryzrange_, false );
    }
}


void uiLogViewTable::setZDomain( uiWellCharts::ZType ztyp )
{
    for ( int idx=0; idx<size(); idx++ )
    {
	uiLogChart* lchart = getLogChart( idx );
	if ( lchart )
	    lchart->setZType( ztyp );
    }

    updatePrimaryZrangeCB( nullptr );
}


void uiLogViewTable::selectCB( CallBacker* )
{
    selectView( logviews_->currentCol() );
}


void uiLogViewTable::addTrackCB( CallBacker* )
{
    int curcol = 0;
    if ( size()>0 && selected_>=0 )
	curcol = selected_;

    const int row = showtools_ ? 1 : 0;
    logviews_->insertColumns( curcol, 1 );
    logviews_->setCurrentCell( RowCol(row, curcol) );
    trackaddCB( nullptr );
}


void uiLogViewTable::removeTrackCB( CallBacker* )
{
    if ( size()==0 || selected_==-1 )
	return;

    auto* logchart = getLogChart( selected_ );
    mDetachCB( logchart->logChange, uiLogViewTable::updatePrimaryChartCB );
    mDetachCB( logchart->plotAreaChanged, uiLogViewTable::alignTopCB );
    mDetachCB( logchart->getZAxis()->rangeChanged,
	       uiLogViewTable::zRangeChangeCB);
    logviews_->removeColumn( selected_ );
    selected_ = -1;
    trackremoveCB( nullptr );
}


void uiLogViewTable::trackaddCB( CallBacker* )
{
    addViewer( logviews_->currentCol() );
    selectView( logviews_->currentCol() );
}


void uiLogViewTable::trackremoveCB( CallBacker* )
{
    updatePrimaryZrangeCB( nullptr );
    updatePrimaryChartCB( nullptr );
}


void uiLogViewTable::colSelectCB( CallBacker* cb )
{
    mCBCapsuleUnpack(int,col,cb);
    if ( selected_==col )
	clearSelection();
    else
	selectView( col );
}

void uiLogViewTable::updatePrimaryChartCB( CallBacker* cb )
{
    mDynamicCastGet(uiLogChart*,logchart,cb)

    if ( !logchart || !primarychart_ )
    {
	primarychart_ = getLogChart( 0 );
	if ( !primarychart_ )
	    return;

	for ( int idx=0; idx<size(); idx++ )
	{
	    logchart = getLogChart( idx );
	    if ( !logchart || logchart->numAxes(OD::Horizontal)==0 ||
		 logchart==primarychart_ )
		continue;

	    if ( logchart->numAxes(OD::Horizontal) >
					primarychart_->numAxes(OD::Horizontal) )
		primarychart_ = logchart;
	}
    }
    else if ( logchart!=primarychart_ && logchart->numAxes(OD::Horizontal)>
					primarychart_->numAxes(OD::Horizontal) )
	primarychart_ = logchart;
    else
	alignTopCB( cb );

    primarychart_->setMargins( 0, 0, 0, 0 );
    alignTopCB( nullptr );
}


void uiLogViewTable::alignTopCB( CallBacker* cb )
{
    if ( !primarychart_ )
	return;

    mDynamicCastGet(uiLogChart*,logchart,cb)
    const Geom::RectF primaryrect = primarychart_->plotArea();
    if ( logchart && logchart!=primarychart_ )
    {
	const Geom::RectI m = logchart->margins();
	const Geom::RectF pa = logchart->plotArea();
	const int topmargin =  m.top() + (primaryrect.top() - pa.top());
	logchart->setMargins( 0, topmargin, 0, 0 );
    }
    else
    {
	for ( int idx=0; idx<size(); idx++ )
	{
	    logchart = getLogChart( idx );
	    if ( !logchart || logchart==primarychart_ )
		continue;
	    const Geom::RectI m = logchart->margins();
	    const Geom::RectF pa = logchart->plotArea();
	    const int topmargin =  m.top() + (primaryrect.top() - pa.top());
	    logchart->setMargins( 0, topmargin, 0, 0 );
	}
    }
}


void uiLogViewTable::updatePrimaryZrangeCB( CallBacker* )
{
    primaryzrange_.setUdf();
    for ( int idx=0; idx<size(); idx++ )
    {
	const uiLogChart* lchart = getLogChart( idx );
	if ( lchart )
	    primaryzrange_.include( lchart->getActualZRange() );
    }

    for ( int idx=0; idx<size(); idx++ )
    {
	uiLogChart* lchart = getLogChart( idx );
	if ( lchart )
	    lchart->getZAxis()->setAxisLimits( primaryzrange_, false );
    }
}


void uiLogViewTable::updateViewLabel( int col )
{
    if ( validIdx( col) )
    {
	uiLogChart* chart = getLogChart( col );
	if ( !chart )
	    return;

	BufferStringSet wellnms = chart->wellNames();
	logviews_->setColumnLabel( col, toUiString(wellnms.cat(" ")) );
    }
}


void uiLogViewTable::addViewer( int col )
{
    const int row = showtools_ ? 1 : 0;
    const RowCol addcell( row, col);
    if ( validIdx(col) && logviews_->getCellObject(addcell) )
	logviews_->clearCellObject( addcell );

    auto* vwr = new uiLogView( nullptr, "viewer" );
    auto* chart = new uiLogChart;
    if ( primaryzrange_.isUdf() )
	primaryzrange_.set( 0, 1000 );

    chart->setZRange( primaryzrange_ );
    vwr->setChart( chart );
    vwr->setLogViewTable( this );
    chart->displayLegend( false );
    chart->setMargins( 0, 0, 0, 0 );
    vwr->setStretch( 2, 2 );
    logviews_->setCellObject( addcell, vwr );
    logviews_->setColumnLabel( addcell.col(), uiString::empty() );
    if ( showtools_ )
	addTools( col );

    mAttachCB( chart->logChange, uiLogViewTable::updatePrimaryChartCB );
    mAttachCB( chart->plotAreaChanged, uiLogViewTable::alignTopCB );
    mAttachCB( chart->getZAxis()->rangeChanged, uiLogViewTable::zRangeChangeCB );
}


void uiLogViewTable::addTools( int col )
{
    const RowCol toolscell( 0, col );
    if ( validIdx(col) && logviews_->getCellGroup(toolscell) )
	return;

    auto* grp = new uiLogViewToolGrp( nullptr, *getLogView(col) );
    logviews_->setCellGroup( toolscell, grp );
}


void uiLogViewTable::clearSelection()
{
    for ( int idx=0; idx<logviews_->nrCols(); idx++ )
    {
	uiLogView* logview = getLogView( idx );
	if ( logview )
	    logview->setBackgroundColor( OD::Color::NoColor() );
    }

    selected_ = -1;
    chartSelectionChg.trigger();
}


void uiLogViewTable::selectView( int col )
{
    {
	NotifyStopper ns( chartSelectionChg );
	clearSelection();
    }
    uiLogView* logview = getLogView( col );
    if ( !logview )
	return;

    logview->setBackgroundColor( OD::Color(205,235,205) );
    selected_ = col;
    logviews_->ensureCellVisible( RowCol(0,col) );
    chartSelectionChg.trigger();
}


bool uiLogViewTable::isViewLocked( int vwidx ) const
{
    if ( alllocked_ )
	return validIdx( vwidx );

    if ( validIdx(vwidx) && showtools_ )
    {
	const RowCol curcell( 0, vwidx );
	mDynamicCastGet(uiLogViewToolGrp*,grp,
			logviews_->getCellGroup(curcell))
	return grp ? grp->isLocked() : false;
    }

    return false;
}


bool uiLogViewTable::isViewVisible( int vwidx ) const
{
    if ( validIdx(vwidx) )
    {
	const int row = showtools_ ? 1 : 0;
	return logviews_->isCellVisible( RowCol(row, vwidx) );
    }

    return false;
}


void uiLogViewTable::setViewLocked( int vwidx, bool yn )
{
    if ( validIdx(vwidx) && showtools_ )
    {
	const RowCol curcell( 0, vwidx );
	mDynamicCastGet(uiLogViewToolGrp*,grp,
			logviews_->getCellGroup(curcell))
	if ( grp )
	    grp->setLocked( yn );
    }
}


void uiLogViewTable::setAllLocked( bool yn )
{
    alllocked_ = yn;
    if ( showtools_ )
    {
	for ( int idx=0; idx<logviews_->nrCols(); idx++ )
	{
	    const RowCol curcell( 0, idx );
	    mDynamicCastGet(uiLogViewToolGrp*,grp,
			    logviews_->getCellGroup(curcell))
	    if ( grp )
	    {
		grp->setLocked( yn );
		grp->hideLock( yn );
	    }
	}
    }
}


void uiLogViewTable::zRangeChangeCB( CallBacker* cb )
{
    mCBCapsuleUnpackWithCaller(const Interval<float>&,range,cber,cb);
    int vwidx = -1;
    for (int idx=0; idx<logviews_->nrCols(); idx++ )
    {
	if ( getLogChart(idx)->getZAxis()==cber )
	{
	    vwidx = idx;
	    break;
	}
    }
    if ( vwidx==-1 || !isViewLocked(vwidx) )
	return;

    syncrange_ = range;
    syncrange_.shift( -getLogChart(vwidx)->zShift() );
    syncview_ = vwidx;
    syncViewsCB( nullptr );
}


void uiLogViewTable::syncViewsCB( CallBacker* )
{
    if ( syncview_==-1 || !isViewLocked(syncview_) )
	return;

    Interval<float> shrange = syncrange_;
    for ( int idx=0; idx<logviews_->nrCols(); idx++ )
    {
	if ( idx!=syncview_ && isViewLocked(idx) && isViewVisible(idx) )
	{
	    uiLogChart* logchart = getLogChart( idx );
	    NotifyStopper ns( logchart->getZAxis()->rangeChanged );
	    logchart->setZRange( shrange );
	    logchart->needsRedraw.trigger();
	}
    }
}
