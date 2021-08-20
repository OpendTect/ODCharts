/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		August 2021
________________________________________________________________________

-*/


#include "uilogviewertree.h"

#include "ioobj.h"
#include "uistrings.h"
#include "uitreeview.h"
#include "welldata.h"
#include "wellman.h"


uiLogViewerTree::uiLogViewerTree( uiParent* p )
    : uiTreeView(p, "LogViewer tree")
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
    showHeader( false );
    setColumnWidthMode( ResizeToContents );
    setSelectionBehavior( SelectRows );
    addWells();
}


void uiLogViewerTree::addWells()
{
    TypeSet<MultiID> wellids;
    Well::MGR().getWellKeys( wellids );
    Well::LoadReqs lreq( Well::Inf );
    for ( auto& wellid : wellids )
    {
	ConstRefMan<Well::Data> wd = Well::MGR().get( wellid, lreq );
	uiTreeViewItem* well_tritem = new uiTreeViewItem( this,
		       uiTreeViewItem::Setup().label(toUiString(wd->name())) );
	addLogs( wellid, well_tritem );
	addMarkers( wellid, well_tritem );
    }
}


void uiLogViewerTree::addLogs( const MultiID& wellid,
						   uiTreeViewItem* well_tritem )
{
    BufferStringSet lognms;
    Well::MGR().getLogNamesByID( wellid, lognms );
    uiTreeViewItem* loglbl_tritem = new uiTreeViewItem( well_tritem,
					     uiTreeViewItem::Setup(
						 uiStrings::sLogs()) );
    for ( auto* lognm : lognms )
    {
	uiTreeViewItem* log_treeitem = new uiTreeViewItem( loglbl_tritem,
					       uiTreeViewItem::Setup(
						    toUiString(*lognm ),
						    uiTreeViewItem::CheckBox,
						    false) );
	mAttachCB(log_treeitem->stateChanged,uiLogViewerTree::logStateChgCB);
    }
}


void uiLogViewerTree::addMarkers( const MultiID& wellid,
						   uiTreeViewItem* well_tritem )
{
    BufferStringSet markernms;
    Well::MGR().getMarkersByID( wellid, markernms );
    uiTreeViewItem* mrklbl_tritem = new uiTreeViewItem( well_tritem,
					     uiTreeViewItem::Setup(
						 uiStrings::sMarker(mPlural),
						 uiTreeViewItem::CheckBox,
						 false) );
    for ( auto* marker : markernms )
    {
	uiTreeViewItem* mrk_treeitem = new uiTreeViewItem( mrklbl_tritem,
					       uiTreeViewItem::Setup(
						    toUiString(*marker ),
						    uiTreeViewItem::CheckBox,
						    false) );
	mAttachCB(mrk_treeitem->stateChanged,uiLogViewerTree::markerStateChgCB);
    }
}


void uiLogViewerTree::checkLogsFor( const MultiID& wellid,
						const BufferStringSet& lognms )
{
    Well::LoadReqs lreq( Well::Inf );
    ConstRefMan<Well::Data> wd = Well::MGR().get( wellid, lreq );
    uiTreeViewItem* well_tritem = findItem( wd->name(), 0, true );
    if ( !well_tritem )
	return;

    uiTreeViewItem* loglbl_tritem = well_tritem->firstChild();
    if ( !loglbl_tritem )
	return;

    loglbl_tritem->checkAll( false, false );
    for ( int idx=0; idx<loglbl_tritem->nrChildren(); idx++ )
    {
	uiTreeViewItem* log_tritem = loglbl_tritem->getChild( idx );
	if ( lognms.isPresent(log_tritem->text()) )
	    log_tritem->setChecked( true, false );
    }
    well_tritem->setOpen( true );
    loglbl_tritem->setOpen( true );
}


void uiLogViewerTree::checkMarkersFor( const MultiID& wellid,
						const BufferStringSet& mrknms )
{
    Well::LoadReqs lreq( Well::Inf );
    ConstRefMan<Well::Data> wd = Well::MGR().get( wellid, lreq );
    uiTreeViewItem* well_tritem = findItem( wd->name(), 0, true );
    if ( !well_tritem )
	return;

    uiTreeViewItem* mrklbl_tritem = well_tritem->lastChild();
    if ( !mrklbl_tritem )
	return;

    mrklbl_tritem->checkAll( false, false );
    for ( int idx=0; idx<mrklbl_tritem->nrChildren(); idx++ )
    {
	uiTreeViewItem* mrk_tritem = mrklbl_tritem->getChild( idx );
	if ( mrknms.isPresent(mrk_tritem->text()) )
	    mrk_tritem->setChecked( true, false );
    }
    well_tritem->setOpen( true );
    mrklbl_tritem->setOpen( true );
}


void uiLogViewerTree::logStateChgCB( CallBacker* cb )
{
    mDynamicCastGet(uiTreeViewItem*,log_tritm,cb->trueCaller());
    if ( log_tritm )
    {
	const BufferString wellname( log_tritm->parent()->parent()->text() );
	const IOObj* ioobj = Well::findIOObj( wellname, nullptr );
	if ( ioobj )
	{
	    const LogID logid( ioobj->key(), log_tritm->text() );
	    log_tritm->isChecked() ? logChecked.trigger(logid)
				    : logUnchecked.trigger(logid);
	}
    }
}


void uiLogViewerTree::markerStateChgCB( CallBacker* cb )
{
    mDynamicCastGet(uiTreeViewItem*,mrk_tritm,cb->trueCaller());
    if ( mrk_tritm )
    {
	const BufferString wellname( mrk_tritm->parent()->parent()->text() );
	const IOObj* ioobj = Well::findIOObj( wellname, nullptr );
	if ( ioobj )
	{
	    const MarkerID mrkid( ioobj->key(), mrk_tritm->text() );
	    mrk_tritm->isChecked() ? markerChecked.trigger(mrkid)
				   : markerUnchecked.trigger(mrkid);
	}
    }
}
