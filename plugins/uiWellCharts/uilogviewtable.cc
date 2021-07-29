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
#include "uimain.h"
#include "uimsg.h"
#include "uitable.h"


uiLogViewTable::uiLogViewTable( uiParent* p )
    : uiGroup(p)
    , masterzrange_(Interval<float>::udf())
{
    logviews_ = new uiTable( this, uiTable::Setup(1,0)
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
    logviews_->setRowResizeMode( uiTable::Stretch );
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

    masterzrange_.setUdf();
}


bool uiLogViewTable::isEmpty() const
{
    return size()==0;
}


void uiLogViewTable::addToViewLabel( int idx, const char* label )
{
    if ( !validIdx(idx) )
	return;

    BufferString bs( logviews_->columnLabel(idx) );
    if ( !bs.contains(label) )
    {
	bs.addSpace().add( label );
	logviews_->setColumnLabel( idx, toUiString(bs) );
    }
}


void uiLogViewTable::setViewLabel( int idx, const uiString& label )
{
    if ( !validIdx(idx) )
	return;

    logviews_->setColumnLabel( idx, label );
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

    const RowCol curcell( 0, col );
    mDynamicCastGet(uiLogView*, logview, logviews_->getCellObject( curcell ) );
    return logview;
}


Interval<float> uiLogViewTable::masterZRange() const
{
    return masterzrange_;
}


void uiLogViewTable::setMasterZRange( const Interval<float> range, bool apply )
{
    if ( !apply && masterzrange_.includes(range) )
	return;

    masterzrange_ = range;
    for ( int idx=0; idx<size(); idx++ )
    {
	uiLogChart* lchart = getLogChart( idx );
	if ( lchart )
	    lchart->getZAxis()->setAxisLimits( masterzrange_, false );
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

    logviews_->insertColumns( curcol, 1 );
    logviews_->setCurrentCell( RowCol(0, curcol) );
    trackaddCB( nullptr );
}


void uiLogViewTable::removeTrackCB( CallBacker* )
{
    if ( size()==0 || selected_==-1 )
	return;

    auto* logchart = getLogChart( selected_ );
    mDetachCB( logchart->plotAreaChanged, uiLogViewTable::updatePlotAreaCB );
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
    updateZrangeCB( nullptr );
    updatePlotAreaCB( nullptr );
}


void uiLogViewTable::colSelectCB( CallBacker* cb )
{
    mCBCapsuleUnpack(int,col,cb);
    if ( selected_==col )
	clearSelection();
    else
	selectView( col );
}

void uiLogViewTable::updatePlotAreaCB( CallBacker* )
{
    uiLogChart* masterchart = getLogChart( 0 );
    if ( !masterchart )
	return;

    for ( int idx=0; idx<size(); idx++ )
    {
	auto* logchart = getLogChart( idx );
	if ( !logchart )
	    continue;
	if ( logchart->numAxes(OD::Horizontal)==0 )
	    return;

	if ( logchart->numAxes(OD::Horizontal)>
					masterchart->numAxes(OD::Horizontal) )
	    masterchart = logchart;
    }

    masterchart->setMargins( 0, 0, 0, 0 );
    const Geom::RectF masterrect = masterchart->plotArea();
    for ( int idx=0; idx<size(); idx++ )
    {
	auto* logchart = getLogChart( idx );
	if ( !logchart || logchart==masterchart )
	    continue;
	NotifyStopper ns( logchart->plotAreaChanged );
	const Geom::RectI m = logchart->margins();
	const Geom::RectF pa = logchart->plotArea();
	const int topmargin =  m.top() + (masterrect.top() - pa.top());
	logchart->setMargins( 0, topmargin, 0, 0 );
    }
    uiMain::repaint();
}


void uiLogViewTable::updateZrangeCB( CallBacker* )
{
    masterzrange_.setUdf();
    for ( int idx=0; idx<size(); idx++ )
    {
	const uiLogChart* lchart = getLogChart( idx );
	if ( lchart )
	    masterzrange_.include( lchart->getActualZRange() );
    }

    for ( int idx=0; idx<size(); idx++ )
    {
	uiLogChart* lchart = getLogChart( idx );
	if ( lchart )
	    lchart->getZAxis()->setAxisLimits( masterzrange_, false );
    }
}


void uiLogViewTable::addViewer( int col )
{
    const RowCol addcell( 0, col);
    if ( validIdx(col) && logviews_->getCellObject(addcell) )
	logviews_->clearCellObject( addcell );

    auto* vwr = new uiLogView( nullptr, "viewer" );
    auto* chart = new uiLogChart;
    if ( masterzrange_.isUdf() )
	masterzrange_.set( 0, 1000);
    chart->setZRange( masterzrange_ );
    vwr->setChart( chart );
    chart->displayLegend( false );
    chart->setMargins( 0, 0, 0, 0 );
    vwr->setStretch( 2, 2 );
    logviews_->setCellObject( addcell, vwr );
    logviews_->setColumnLabel( addcell.second, uiString::empty() );
    mAttachCB(chart->plotAreaChanged, uiLogViewTable::updatePlotAreaCB);
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
}

void uiLogViewTable::selectView( int col )
{
    clearSelection();
    uiLogView* logview = getLogView( col );
    if ( logview )
    {
	logview->setBackgroundColor( OD::Color::Red() );
	selected_ = col;
	logviews_->ensureCellVisible( RowCol(0,col) );
    }
}
