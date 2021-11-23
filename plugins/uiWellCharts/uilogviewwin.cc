/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		November 2020
________________________________________________________________________

-*/

#include "uilogviewwin.h"

#include "uichartaxes.h"
#include "uifiledlg.h"
#include "uilogchart.h"
#include "uilogview.h"
#include "uilogviewertree.h"
#include "uilogviewtable.h"
#include "uimain.h"
#include "uimsg.h"
#include "uiscrollarea.h"
#include "uisplitter.h"
#include "uitable.h"
#include "uitoolbar.h"
#include "uitoolbutton.h"

#include "commandlineparser.h"
#include "filepath.h"
#include "iopar.h"
#include "oddirs.h"
#include "welldata.h"
#include "wellman.h"
#include "welltrack.h"

#include <QChart>

static int sMnuID = 0;
static const int sWinHeight = 500;
static const int sWinWidth = 500;

uiLogViewWin::uiLogViewWin( uiParent* p )
    : uiMainWin(p,toUiString("OpendTect - Log Viewer"))
    , newitem_(uiStrings::sNew(),"new","",
				mCB(this,uiLogViewWin,newCB),sMnuID++)
    , openitem_(uiStrings::sOpen(),"open","",
				mCB(this,uiLogViewWin,openCB),sMnuID++)
    , saveitem_(uiStrings::sSave(),"save","",
				mCB(this,uiLogViewWin,saveCB),sMnuID++)
    , saveasitem_(uiStrings::sSaveAs(),"saveas","",
				mCB(this,uiLogViewWin,saveasCB),sMnuID++)
    , addbuttonitem_(uiStrings::sAdd(),"plus","",
				mCB(this,uiLogViewWin,addTrackCB),sMnuID++)
    , rmvbuttonitem_(uiStrings::sRemove(),"minus","",
				mCB(this,uiLogViewWin,rmvTrackCB),sMnuID++)
{
    mainObject()->setMinimumHeight( sWinHeight );
    mainObject()->setMinimumWidth( sWinWidth );
    createToolBar();

    logviewtree_ = new uiLogViewerTree( this );
    logviewtree_->setStretch( 0, 2 );

    logviewtbl_ = new uiLogViewTable( this, true );
    logviewtbl_->attach( rightTo, logviewtree_ );

    mAttachCB( postFinalise(), uiLogViewWin::uiInitCB );
}


uiLogViewWin::~uiLogViewWin()
{
    detachAllNotifiers();
}


void uiLogViewWin::uiInitCB( CallBacker* )
{
    mAttachCB( windowClosed, uiLogViewWin::closeCB );
    mAttachCB( logviewtbl_->chartSelectionChg, uiLogViewWin::selTrackChgCB );
    mAttachCB( logviewtree_->logChecked, uiLogViewWin::addLogCB );
    mAttachCB( logviewtree_->logUnchecked, uiLogViewWin::removeLogCB );
    mAttachCB( logviewtree_->markerChecked, uiLogViewWin::addMarkerCB );
    mAttachCB( logviewtree_->markerUnchecked, uiLogViewWin::removeMarkerCB );

    CommandLineParser clp;
    if ( this==uiMain::theMain().topLevel() && !clp.hasKey("odserver") )
    {
        BufferString parfile;
	if ( clp.getVal("lvpar",parfile) )
	    loadFile( parfile );
	else
	{
	    BufferStringSet wellids, welllogs;
	    clp.getVal( "well", wellids );
	    clp.getVal( "welllogs", welllogs );
	    loadWells( wellids, welllogs );
	}
    }
}


void uiLogViewWin::createToolBar()
{
    tb_ = new uiToolBar( this, tr("Log Viewer") );
    tb_->addButton( newitem_ );
    tb_->addButton( openitem_ );
    tb_->addButton( saveitem_ );
    tb_->addButton( saveasitem_ );
    tb_->addSeparator();
    tb_->addButton( addbuttonitem_ );
    tb_->addButton( rmvbuttonitem_ );
    tb_->addSeparator();
}


bool uiLogViewWin::checkSave()
{
    if ( logviewtbl_->size()>0 && needsave_ )
    {
	uiString msg = tr("Current display has not been saved.\n"
			  "Do you want to save it?");
	const int res = uiMSG().askSave( msg, false );
	if ( res == 1 )
	    saveasCB( nullptr );
    }

    return true;
}


void uiLogViewWin::add( int idx, const MultiID& wellkey,
			const BufferStringSet& lognms )
{
    uiLogChart* chart = logviewtbl_->getLogChart( idx );
    if ( !chart )
	return;

    for ( auto* lognm : lognms )
	addLog( idx, wellkey, *lognm );
}


void uiLogViewWin::addLog( int idx, const MultiID& wellkey,
			   const BufferString& lognm )
{
    uiLogChart* chart = logviewtbl_->getLogChart( idx );
    if ( !chart )
	return;

    if ( chart->hasLogCurve(wellkey,lognm) )
	return;

    chart->addLogCurve( wellkey, lognm );

    Well::LoadReqs lreq( Well::Trck );
    RefMan<Well::Data> wd = Well::MGR().get( wellkey, lreq );
    logviewtbl_->updateViewLabel( idx );
    if ( logviewtbl_->primaryZRange().includes(wd->track().zRange()) )
	chart->getZAxis()->setAxisLimits( logviewtbl_->primaryZRange() );
    else
	logviewtbl_->updatePrimaryZrangeCB( nullptr );

    needsave_ = true;
}


void uiLogViewWin::rmvLog( int idx, const MultiID& wellkey,
			   const BufferString& lognm )
{
    uiLogChart* chart = logviewtbl_->getLogChart( idx );
    if ( !chart )
	return;

    if ( !chart->hasLogCurve(wellkey,lognm) )
	return;

    chart->removeLogCurve( wellkey, lognm );
    logviewtbl_->updateViewLabel( idx );
    logviewtbl_->updatePrimaryZrangeCB( nullptr );

    needsave_ = true;
}


void uiLogViewWin::addLogCB( CallBacker* cb )
{
    mCBCapsuleUnpack(const LogID&,logid,cb);
    const MultiID wellid = logid.first;
    const BufferString lognm( logid.second );

    if ( logviewtbl_->isEmpty() )
    {
	NotifyStopper ns( logviewtbl_->chartSelectionChg );
	logviewtbl_->addTrackCB( nullptr );
    }

    addLog( logviewtbl_->currentView(), wellid, lognm );
}


void uiLogViewWin::removeLogCB( CallBacker* cb )
{
    mCBCapsuleUnpack(const LogID&,logid,cb);
    const MultiID wellid = logid.first;
    const BufferString lognm( logid.second );

    rmvLog( logviewtbl_->currentView(), wellid, lognm );
}


void uiLogViewWin::addMarker( int idx, const MultiID& wellkey,
			   const BufferString& markernm )
{
    uiLogChart* chart = logviewtbl_->getLogChart( idx );
    if ( !chart )
	return;

    if ( chart->hasMarker(wellkey, markernm) )
	return;

    chart->addMarker( wellkey, markernm );

    logviewtbl_->updateViewLabel( idx );
    needsave_ = true;
}


void uiLogViewWin::rmMarker( int idx, const MultiID& wellkey,
			     const BufferString& markernm )
{
    uiLogChart* chart = logviewtbl_->getLogChart( idx );
    if ( !chart )
	return;

    if ( !chart->hasMarker(wellkey, markernm) )
	return;

    chart->removeMarker( wellkey, markernm );
    logviewtbl_->updateViewLabel( idx );
    needsave_ = true;
}


void uiLogViewWin::addMarkerCB( CallBacker* cb )
{
    mCBCapsuleUnpack(const MarkerID&,markerid,cb);
    const MultiID wellid = markerid.first;
    const BufferString markernm( markerid.second );

    if ( logviewtbl_->isEmpty() )
    {
	NotifyStopper ns( logviewtbl_->chartSelectionChg );
	logviewtbl_->addTrackCB( nullptr );
    }

    addMarker( logviewtbl_->currentView(), wellid, markernm );
}


void uiLogViewWin::removeMarkerCB( CallBacker* cb )
{
    mCBCapsuleUnpack(const MarkerID&,markerid,cb);
    const MultiID wellid = markerid.first;
    const BufferString markernm( markerid.second );

    rmMarker( logviewtbl_->currentView(), wellid, markernm );
}

void uiLogViewWin::clearAll()
{
    logviewtbl_->setEmpty();
    logviewtree_->uncheckAll();
    logviewtree_->collapseAll();
}


void uiLogViewWin::addWellData( const DBKey& wellkey, const TypeSet<int>& logs )
{
    addTrackCB( nullptr );
    const int vwidx = logviewtbl_->currentView();
    BufferStringSet lognms;
    Well::Man::getLogNamesByID( wellkey, lognms );
    for ( const auto& lidx : logs )
    {
	if ( !lognms.validIdx(lidx) )
	    continue;

	addLog( vwidx, wellkey, lognms.get(lidx) );
    }
}


void uiLogViewWin::loadWells( const BufferStringSet& wellids,
			      const BufferStringSet& logids )
{
    if ( wellids.size() != logids.size() )
	return;

    for ( int idx=0; idx<wellids.size(); idx++ )
    {
	const DBKey wellkey( MultiID(wellids.get(idx)) );
	BufferStringSet lognums;
	lognums.unCat( logids.get(idx), "," );
	TypeSet<int> logs;
	for ( const auto* logid : lognums )
	    logs += logid->toInt();

	addWellData( wellkey, logs );
    }
}


void uiLogViewWin::loadFile( const char* nm )
{
    clearAll();

    filename_ = nm;
    IOPar iop;
    if ( iop.read(filename_,"Log Display",true) )
    {
	int nitems = 0;
	iop.get( sKey::NrItems(), nitems );
	if ( nitems<1 )
	    return;

	for ( int idx=nitems-1; idx>=0; idx-- )
	{
	    addTrackCB( nullptr );
	    IOPar* tmp = iop.subselect( IOPar::compKey(sKey::ID(),idx) );
	    uiLogChart* chart = logviewtbl_->getCurrentLogChart();
	    if ( !chart )
		return;

	    chart->usePar( *tmp );
	    logviewtbl_->updateViewLabel( logviewtbl_->currentView() );
	}

	logviewtbl_->clearSelection();
	logviewtbl_->updatePrimaryZrangeCB( nullptr );
	needsave_ = false;
    }
}


void uiLogViewWin::newCB( CallBacker* )
{
    if ( !checkSave() )
	return;

    clearAll();
}


void uiLogViewWin::openCB( CallBacker* )
{
    if ( !checkSave() )
	return;

    const BufferString defseldir =
	FilePath( GetDataDir() ).add( defDirStr() ).fullPath();
    uiFileDialog dlg( this, true, 0, filtStr(),
		      tr("Load Log View parameters") );
    dlg.setDirectory(defseldir);
    if ( !dlg.go() )
	return;

    loadFile( dlg.fileName() );
}


void uiLogViewWin::saveCB( CallBacker* )
{
    if ( filename_.isEmpty() )
	saveasCB( nullptr );

    const int nrviews = logviewtbl_->size();
    if ( nrviews<1 )
	return;

    IOPar iop;
    iop.set( sKey::NrItems(), nrviews );

    for ( int idx=0; idx<nrviews; idx++ )
    {
	IOPar tmp;
	logviewtbl_->getLogChart(idx)->fillPar( tmp );
	iop.mergeComp( tmp, IOPar::compKey(sKey::ID(),idx) );
    }

    iop.write( filename_, "Log Display" );
    needsave_ = false;
}


void uiLogViewWin::saveasCB( CallBacker* )
{
    if ( logviewtbl_->isEmpty() )
	return;

    const BufferString defseldir =
		FilePath( GetDataDir() ).add( defDirStr() ).fullPath();
    uiFileDialog dlg( this, false, 0, filtStr(),
		      tr("Save Log View parameters") );
    dlg.setMode( uiFileDialog::AnyFile );
    dlg.setDirectory( defseldir );
    dlg.setDefaultExtension( extStr() );
    dlg.setConfirmOverwrite( true );
    dlg.setSelectedFilter( filtStr() );
    if ( !dlg.go() )
	return;

    filename_ = dlg.fileName();
    raise();
    saveCB( nullptr );
}


void uiLogViewWin::addTrackCB( CallBacker* )
{
    logviewtbl_->addTrackCB( nullptr );
    needsave_ = true;
}


void uiLogViewWin::rmvTrackCB( CallBacker* )
{
    logviewtbl_->removeTrackCB( nullptr );
    selTrackChgCB( nullptr );
    needsave_ = true;
}


void uiLogViewWin::closeCB( CallBacker* )
{
   checkSave();
}


void uiLogViewWin::selTrackChgCB( CallBacker* )
{
    NotifyStopper ns1(logviewtree_->logChecked);
    NotifyStopper ns2(logviewtree_->logUnchecked);
    NotifyStopper ns3(logviewtree_->markerChecked);
    NotifyStopper ns4(logviewtree_->markerUnchecked);
    logviewtree_->uncheckAll();
    logviewtree_->collapseAll();
    uiLogChart* logchart = logviewtbl_->getCurrentLogChart();
    if ( !logchart )
	return;

    TypeSet<MultiID> wellids = logchart->wellIDs();
    for ( const auto& wellid : wellids )
    {
	const BufferStringSet logs = logchart->getDispLogsForID( wellid );
	logviewtree_->checkLogsFor( wellid, logs );
	const BufferStringSet mrkrs = logchart->getDispMarkersForID( wellid );
	logviewtree_->checkMarkersFor( wellid, mrkrs );
    }
}
