/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		August 2021
________________________________________________________________________

-*/


#include "uilogviewertree.h"

#include "uistrings.h"
#include "uitreeview.h"

#include "ioobj.h"
#include "welldata.h"
#include "wellman.h"
#include "uimsg.h"


uiLogViewerTree::uiLogViewerTree( uiParent* p )
    : uiGroup(p)
    , logChecked(this)
    , logUnchecked(this)
    , markerChecked(this)
    , markerUnchecked(this)
{
    init();
}


uiLogViewerTree::~uiLogViewerTree()
{
    detachAllNotifiers();
}


void uiLogViewerTree::init()
{
    treeview_ = new uiTreeView( this );
    treeview_->showHeader( false );
    treeview_->setColumnWidthMode( uiTreeView::ResizeToContents );
    treeview_->setSelectionBehavior( uiTreeView::SelectRows );
    treeview_->setStretch( 2, 2 );
    addWells();
    mAttachCB( treeview_->expanded, uiLogViewerTree::wellExpandCB );
}


void uiLogViewerTree::addWells()
{
    BufferStringSet wellnms;
    Well::MGR().getWellNames( wellnms );
    for ( const auto* wellnm : wellnms )
    {
	auto* well_tritem = new uiTreeViewItem( treeview_,
		uiTreeViewItem::Setup().label(toUiString(wellnm->buf())) );
	new uiTreeViewItem( well_tritem,
			    uiTreeViewItem::Setup(uiStrings::sLogs()) );
	auto* mrklbl_tritem = new uiTreeViewItem( well_tritem,
			uiTreeViewItem::Setup(uiStrings::sMarker(mPlural),
					      uiTreeViewItem::CheckBox,false) );
	mAttachCB( mrklbl_tritem->stateChanged,
		   uiLogViewerTree::allMarkerStateChgCB);
    }
}


void uiLogViewerTree::addLogs( const MultiID& wellid,
			       uiTreeViewItem* well_tritem )
{
    BufferStringSet lognms;
    Well::MGR().getLogNamesByID( wellid, lognms );
    auto* loglbl_tritem = well_tritem->firstChild();
    if ( !loglbl_tritem || loglbl_tritem->nrChildren()==lognms.size() )
	return;

    for ( auto* lognm : lognms )
    {
	auto* log_treeitem = new uiTreeViewItem( loglbl_tritem,
			uiTreeViewItem::Setup(toUiString(*lognm),
					      uiTreeViewItem::CheckBox,false) );
	mAttachCB( log_treeitem->stateChanged,
		   uiLogViewerTree::logStateChgCB );
    }
}


void uiLogViewerTree::addMarkers( const MultiID& wellid,
				  uiTreeViewItem* well_tritem )
{
    BufferStringSet markernms;
    Well::MGR().getMarkersByID( wellid, markernms );
    uiTreeViewItem* mrklbl_tritem = well_tritem->lastChild();
    if ( !mrklbl_tritem || mrklbl_tritem->nrChildren()==markernms.size() )
	return;

    for ( auto* marker : markernms )
    {
	auto* mrk_treeitem = new uiTreeViewItem( mrklbl_tritem,
			uiTreeViewItem::Setup(toUiString(*marker ),
					      uiTreeViewItem::CheckBox,false) );
	mAttachCB( mrk_treeitem->stateChanged,
		   uiLogViewerTree::markerStateChgCB );
    }
}


void uiLogViewerTree::checkLogsFor( const MultiID& wellid,
				    const BufferStringSet& lognms )
{
    Well::LoadReqs lreq( Well::Inf );
    ConstRefMan<Well::Data> wd = Well::MGR().get( wellid, lreq );
    uiTreeViewItem* well_tritem = treeview_->findItem( wd->name(), 0, true );
    if ( !well_tritem )
	return;

    uiTreeViewItem* loglbl_tritem = well_tritem->firstChild();
    if ( !loglbl_tritem )
	return;

    if ( !loglbl_tritem->nrChildren() )
	addLogs( wellid, well_tritem );

    for ( int idx=0; idx<loglbl_tritem->nrChildren(); idx++ )
    {
	uiTreeViewItem* log_tritem = loglbl_tritem->getChild( idx );
	log_tritem->setChecked( lognms.isPresent(log_tritem->text()), false );
    }

    well_tritem->setOpen( true );
    loglbl_tritem->setOpen( true );
}


void uiLogViewerTree::checkMarkersFor( const MultiID& wellid,
						const BufferStringSet& mrknms )
{
    Well::LoadReqs lreq( Well::Inf );
    ConstRefMan<Well::Data> wd = Well::MGR().get( wellid, lreq );
    uiTreeViewItem* well_tritem = treeview_->findItem( wd->name(), 0, true );
    if ( !well_tritem )
	return;

    uiTreeViewItem* mrklbl_tritem = well_tritem->lastChild();
    if ( !mrklbl_tritem )
	return;

    if ( !mrklbl_tritem->nrChildren() )
	addMarkers( wellid, well_tritem );

    for ( int idx=0; idx<mrklbl_tritem->nrChildren(); idx++ )
    {
	uiTreeViewItem* mrk_tritem = mrklbl_tritem->getChild( idx );
	mrk_tritem->setChecked( mrknms.isPresent(mrk_tritem->text()), false );
    }

    well_tritem->setOpen( true );
    mrklbl_tritem->setOpen( true );
}


void uiLogViewerTree::logStateChgCB( CallBacker* cb )
{
    mDynamicCastGet(uiTreeViewItem*,log_tritm,cb->trueCaller());
    if ( !log_tritm )
	return;

    const BufferString wellname( log_tritm->parent()->parent()->text() );
    const IOObj* ioobj = Well::findIOObj( wellname, nullptr );
    if ( ioobj )
    {
	const LogID logid( ioobj->key(), log_tritm->text() );
	log_tritm->isChecked() ? logChecked.trigger(logid)
			       : logUnchecked.trigger(logid);
    }
}


void uiLogViewerTree::markerStateChgCB( CallBacker* cb )
{
    mDynamicCastGet(uiTreeViewItem*,mrk_tritm,cb->trueCaller());
    if ( !mrk_tritm )
	return;

    const BufferString wellname( mrk_tritm->parent()->parent()->text() );
    const IOObj* ioobj = Well::findIOObj( wellname, nullptr );
    if ( ioobj )
    {
	const MarkerID mrkid( ioobj->key(), mrk_tritm->text() );
	mrk_tritm->isChecked() ? markerChecked.trigger(mrkid)
			       : markerUnchecked.trigger(mrkid);
    }
}


void uiLogViewerTree::allMarkerStateChgCB( CallBacker* cb )
{
    mDynamicCastGet(uiTreeViewItem*,mrklbl_tritm,cb->trueCaller());
    if ( !mrklbl_tritm )
	return;

    const bool yn = mrklbl_tritm->isChecked();
    for ( int idx=0; idx<mrklbl_tritm->nrChildren(); idx++ )
	mrklbl_tritm->getChild(idx)->checkAll( yn, true );
}


void uiLogViewerTree::wellExpandCB( CallBacker* cb )
{
    mDynamicCastGet(uiTreeView*,welltree,cb->trueCaller());
    if ( !welltree )
	return;

    auto* well_tritem = welltree->itemNotified();
    if ( !well_tritem || well_tritem->parent() )
	return;

    const IOObj* ioobj = Well::findIOObj( well_tritem->text(), nullptr );
    if ( !ioobj )
	return;

    const MultiID& wid = ioobj->key();
    addLogs( wid, well_tritem );
    addMarkers( wid, well_tritem );
}


void uiLogViewerTree::uncheckAll()
{
    treeview_->uncheckAll();
}


void uiLogViewerTree::collapseAll()
{
    treeview_->collapseAll();
}
