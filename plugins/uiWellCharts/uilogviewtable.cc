/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		November 2020
________________________________________________________________________

-*/


#include "uilogviewtable.h"

#include "uichartaxes.h"
#include "uilogchart.h"
#include "uilogview.h"
#include "uilogviewtoolgrp.h"
#include "uimain.h"
#include "uimsg.h"
#include "uipixmap.h"
#include "uitable.h"


uiLogViewTable::uiLogViewTable( uiParent* p, bool showtools )
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


    mAttachCB( logviews_->colInserted, uiLogViewTable::trackaddCB );
    mAttachCB( logviews_->colDeleted, uiLogViewTable::trackremoveCB );
    mAttachCB( logviews_->columnClicked, uiLogViewTable::colSelectCB );
    mAttachCB( logviews_->leftClicked, uiLogViewTable::selectCB );
}


uiLogViewTable::~uiLogViewTable()
{
    detachAllNotifiers();
}


int uiLogViewTable::currentView() const
{
    return logviews_->currentCol();
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
    for ( int idx=size()-1; idx>=0; idx-- )
	logviews_->removeColumn( idx );

    primaryzrange_.setUdf();
}


bool uiLogViewTable::isEmpty() const
{
    return size()==0;
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
    mDynamicCastGet(uiLogView*, logview, logviews_->getCellObject( curcell ) );
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
    mDetachCB(logchart->logChange, uiLogViewTable::updatePrimaryChartCB);
    mDetachCB(logchart->plotAreaChanged, uiLogViewTable::alignTopCB);
    mDetachCB(logchart->getZAxis()->rangeChanged, uiLogViewTable::syncViewsCB);
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

void uiLogViewTable::updatePrimaryChartCB( CallBacker* )
{
    if ( !primarychart_ )
	primarychart_ = getLogChart( 0 );

    for ( int idx=0; idx<size(); idx++ )
    {
	auto* logchart = getLogChart( idx );
	if ( !logchart || logchart->numAxes(OD::Horizontal)==0 ||
	     logchart==primarychart_ )
	    continue;

	if ( logchart->numAxes(OD::Horizontal)>
					primarychart_->numAxes(OD::Horizontal) )
	    primarychart_ = logchart;
    }
    primarychart_->setMargins( 0, 0, 0, 0 );
    alignTopCB( nullptr );
}


void uiLogViewTable::alignTopCB( CallBacker* cb )
{
    if ( !primarychart_ )
	return;

    mDynamicCastGet(uiLogChart*, logchart, cb);
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
    chart->displayLegend( false );
    chart->setMargins( 0, 0, 0, 0 );
    vwr->setStretch( 2, 2 );
    logviews_->setCellObject( addcell, vwr );
    logviews_->setColumnLabel( addcell.second, uiString::empty() );
    if ( showtools_ )
	addTools( col );

    mAttachCB( chart->logChange, uiLogViewTable::updatePrimaryChartCB );
    mAttachCB( chart->plotAreaChanged, uiLogViewTable::alignTopCB );
    mAttachCB( chart->getZAxis()->rangeChanged, uiLogViewTable::syncViewsCB );
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
	    logview->setBackgroundColor( Color::NoColor() );
    }

    selected_ = -1;
    chartSelectionChg.trigger();
}


void uiLogViewTable::selectView( int col )
{
    clearSelection();
    uiLogView* logview = getLogView( col );
    if ( !logview )
	return;

    logview->setBackgroundColor( Color(205,235,205) );
    selected_ = col;
    logviews_->ensureCellVisible( RowCol(0,col) );
    chartSelectionChg.trigger();
}


bool uiLogViewTable::isViewLocked( int vwidx )
{
    if ( validIdx(vwidx) && showtools_ )
    {
	const RowCol curcell( 0, vwidx );
	mDynamicCastGet(uiLogViewToolGrp*,grp,
			logviews_->getCellGroup(curcell));
	return grp ? grp->isLocked() : false;
    }

    return false;
}


void uiLogViewTable::setViewLocked( int vwidx, bool yn )
{
    if ( validIdx(vwidx) && showtools_ )
    {
	const RowCol curcell( 0, vwidx );
	mDynamicCastGet(uiLogViewToolGrp*,grp,
			logviews_->getCellGroup(curcell));
	if ( grp )
	    grp->setLocked( yn );
    }
}


void uiLogViewTable::syncViewsCB( CallBacker* cb )
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

    if ( !isViewLocked(vwidx) )
	return;

    for ( int idx=0; idx<logviews_->nrCols(); idx++ )
    {
	if ( idx!=vwidx && isViewLocked(idx) )
	{
	    uiLogChart* logchart = getLogChart( idx );
	    NotifyStopper ns( logchart->getZAxis()->rangeChanged );
	    logchart->setZRange( range );
	    logchart->needsRedraw.trigger();
	}
    }
}
