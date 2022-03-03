/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		November 2020
________________________________________________________________________

-*/

#include "uilogviewwin.h"

#include "logcurve.h"
#include "uichartaxes.h"
#include "uicombobox.h"
#include "uifiledlg.h"
#include "uilogchart.h"
#include "uilogview.h"
#include "uilogviewpropdlg.h"
#include "uilogviewertree.h"
#include "uilogviewtable.h"
#include "uimain.h"
#include "uimsg.h"
#include "uiscrollarea.h"
#include "uisplitter.h"
#include "uitable.h"
#include "uitabstack.h"
#include "uitoolbar.h"
#include "uitoolbutton.h"
#include "uiwellfiltergrp.h"

#include "commandlineparser.h"
#include "filepath.h"
#include "dbkey.h"
#include "iopar.h"
#include "manobjectset.h"
#include "oddirs.h"
#include "survinfo.h"
#include "welldata.h"
#include "wellman.h"
#include "welltrack.h"

#include <QChart>

static int sMnuID = 0;
static const int sWinHeight = 500;
static const int sWinWidth = 500;


uiLogViewWinBase::uiLogViewWinBase( uiParent* p, int nrcol, bool showtools )
    : uiDialog(p,Setup(toUiString("OpendTect - Log Viewer"), mNoDlgTitle,
		       mTODOHelpKey).modal(false))
    , newitem_(uiStrings::sNew(),"new","",
				mCB(this,uiLogViewWinBase,newCB),sMnuID++)
    , openitem_(uiStrings::sOpen(),"open","",
				mCB(this,uiLogViewWinBase,openCB),sMnuID++)
    , saveitem_(uiStrings::sSave(),"save","",
				mCB(this,uiLogViewWinBase,saveCB),sMnuID++)
    , saveasitem_(uiStrings::sSaveAs(),"saveas","",
				mCB(this,uiLogViewWinBase,saveasCB),sMnuID++)
{
    mainObject()->setMinimumHeight( sWinHeight );
    mainObject()->setMinimumWidth( sWinWidth );
    setCtrlStyle( CloseOnly );

    logviewtbl_ = new uiLogViewTable( this, nrcol, showtools );

    mAttachCB( postFinalise(), uiLogViewWinBase::uiInitCB );
}


uiLogViewWinBase::~uiLogViewWinBase()
{
    detachAllNotifiers();
}


void uiLogViewWinBase::createToolBar()
{
    tb_ = new uiToolBar( this, tr("Log Viewer") );
    tb_->addButton( newitem_ );
    tb_->addButton( openitem_ );
    tb_->addButton( saveitem_ );
    tb_->addButton( saveasitem_ );
    tb_->addSeparator();
    addApplicationToolBar();
    tb_->addSeparator();

    EnumDef def = uiWellCharts::ZTypeDef();
    if ( !SI().zIsTime() )
	def.remove( def.getKeyForIndex(uiWellCharts::TWT) );

    zdomainfld_ = new uiComboBox( nullptr, def, "Z domain" );
    mAttachCB( zdomainfld_->selectionChanged, uiLogViewWinBase::zdomainChgCB );
    tb_->addObject( zdomainfld_);
}


void uiLogViewWinBase::uiInitCB( CallBacker* )
{
    mAttachCB( windowClosed, uiLogViewWin::closeCB );
    mAttachCB( logviewtbl_->chartSelectionChg, uiLogViewWinBase::selTrackChgCB );

    CommandLineParser clp;
    if ( this==uiMain::instance().topLevel() && !clp.hasKey("odserver") )
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


bool uiLogViewWinBase::checkSave()
{
    if ( logviewtbl_->size()>0 && needsave_ )
    {
	uiString msg = tr("Current display has not been saved.\n"
			  "Do you want to save it?");
	const int res = uiMSG().askSave( msg, false );
	if ( res == 1 )
	    saveasCB( nullptr );
	else
	    needsave_ = false;
    }

    return true;
}


void uiLogViewWinBase::setCurrentView( int vwidx )
{
    logviewtbl_->setCurrentView( vwidx );
}


void uiLogViewWinBase::clearAll()
{
    logviewtbl_->setEmpty();
}


void uiLogViewWinBase::addWellData( const DBKeySet& wellids,
				const ManagedObjectSet<TypeSet<int>>& logidxs )
{
    logviewtbl_->addWellData( wellids, logidxs );
    needsave_ = true;
}


void uiLogViewWinBase::addWellData( const DBKeySet& wellids,
				    const BufferStringSet& lognms,
				    const BufferStringSet& mrknms )
{
    logviewtbl_->addWellData( wellids, lognms, mrknms );
    needsave_ = true;
}


void uiLogViewWinBase::newCB( CallBacker* )
{
    if ( !checkSave() )
	return;

    clearAll();
}


void uiLogViewWinBase::openCB( CallBacker* )
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


void uiLogViewWinBase::saveCB( CallBacker* )
{
    if ( filename_.isEmpty() )
	saveasCB( nullptr );

    const int nrviews = logviewtbl_->size();
    if ( nrviews<1 )
	return;

    saveFile( filename_ );IOPar iop;
    needsave_ = false;
}


void uiLogViewWinBase::saveasCB( CallBacker* )
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


void uiLogViewWinBase::closeCB( CallBacker* )
{
   checkSave();
}


void uiLogViewWinBase::zdomainChgCB( CallBacker* )
{
    const uiWellCharts::ZType ztyp =
	uiWellCharts::ZTypeDef().getEnumForIndex( zdomainfld_->getIntValue() );
    logviewtbl_->setZDomain( ztyp );
}


uiLockedLogViewWin::uiLockedLogViewWin( uiParent* p,
					const ObjectSet<Well::Data>& wds,
					const BufferStringSet& lognms,
					const BufferStringSet& markernms )
    : uiLogViewWinBase(p, 0, false)
    , settingsbuttonitem_(uiStrings::sSettings(),"settings","",
			mCB(this,uiLockedLogViewWin,showSettingsCB),sMnuID++)
    , unzoombuttonitem_(tr("View All Z"),"view_all","",
			    mCB(this,uiLockedLogViewWin,zoomResetCB),sMnuID++)
{
    createToolBar();

    uiGroup* filtergrp = new uiGroup( this );
    uiPushButton* applybut = new uiPushButton( filtergrp, uiStrings::sApply(),
					       true );
    applybut->setStretch( 2, 0);

    logfiltergrp_ = new uiWellFilterGrp( filtergrp, wds, lognms, markernms,
					 OD::Vertical );
    logfiltergrp_->attach( alignedBelow, applybut );
    mAttachCB(applybut->activated, uiLockedLogViewWin::dataChgCB);
    filtergrp->setStretch( 0, 2 );
    filtergrp->attach( leftOf, logviewtbl_ );
}


uiLockedLogViewWin::~uiLockedLogViewWin()
{
    detachAllNotifiers();
    delete propdlg_;
}


void uiLockedLogViewWin::addApplicationToolBar()
{
    tb_->addButton( settingsbuttonitem_ );
    tb_->addButton( unzoombuttonitem_ );
}


void uiLockedLogViewWin::setSelected( const DBKeySet& wellids,
				      const BufferStringSet& logs,
				      const BufferStringSet& mrkrs )
{
    addWellData( wellids, logs, mrkrs );
    logviewtbl_->setAllLocked( true );
    logfiltergrp_->setSelected( wellids, logs, mrkrs );
}


void uiLockedLogViewWin::dataChgCB( CallBacker* )
{
    if ( propdlg_ )
    {
	propdlg_->close();
	deleteAndZeroPtr( propdlg_ );
    }
    if ( !checkSave() )
	return;

    DBKeySet wellids;
    BufferStringSet lognms, mrknms;
    logfiltergrp_->getSelected( wellids, lognms, mrknms );
    clearAll();
    addWellData( wellids, lognms, mrknms );
    logviewtbl_->setAllLocked( true );
}


void uiLockedLogViewWin::showSettingsCB( CallBacker* )
{
    auto* logchart = logviewtbl_->getCurrentLogChart();
    if ( !logchart )
    {
	uiMSG().message( tr("Please select a log chart") );
	return;
    }
    if ( !propdlg_ )
    {
	propdlg_ = new uiLogViewPropDlg( parent(), logchart, true );
	mAttachCB(propdlg_->applyPushed, uiLockedLogViewWin::applySettingsCB);
	mAttachCB(logviewtbl_->chartSelectionChg,
		  uiLockedLogViewWin::chartChgCB);
    }
    else
	propdlg_->setLogChart( logchart );

    propdlg_->show();
}


void uiLockedLogViewWin::applySettingsCB( CallBacker* )
{
    if ( !propdlg_ )
	return;

    auto* curlogchart = logviewtbl_->getCurrentLogChart();
    uiUserShowWait uisw( this, tr("Working: 0%") );
    IOPar settings = propdlg_->getCurrentSettings();
    for ( int idx=0; idx<logviewtbl_->size(); idx++ )
    {
	auto* logchart = logviewtbl_->getLogChart( idx );
	if ( !logchart || logchart==curlogchart )
	    continue;
	logchart->usePar( settings, true );
	uisw.setMessage(
	    tr("Working: %1 %").arg(int((idx+1)/logviewtbl_->size()*100.f)) );
    }
}


void uiLockedLogViewWin::chartChgCB( CallBacker* )
{
    auto* logchart = logviewtbl_->getCurrentLogChart();
    if ( !propdlg_ || !logchart )
	return;

    propdlg_->setLogChart( logchart );
}


void uiLockedLogViewWin::zoomResetCB( CallBacker* )
{
    for ( int idx=0; idx<logviewtbl_->size(); idx++ )
	logviewtbl_->getLogView( idx )->zoomResetCB( nullptr );
}


uiLogViewWin::uiLogViewWin( uiParent* p, int nrcol )
    : uiLogViewWinBase(p, nrcol)
    , addbuttonitem_(uiStrings::sAdd(),"plus","",
				mCB(this,uiLogViewWin,addTrackCB),sMnuID++)
    , rmvbuttonitem_(uiStrings::sRemove(),"minus","",
				mCB(this,uiLogViewWin,rmvTrackCB),sMnuID++)
{
    createToolBar();

    logviewtree_ = new uiLogViewerTree( this );
    logviewtree_->setStretch( 0, 2 );
    logviewtree_->attach( leftOf, logviewtbl_ );
}


uiLogViewWin::~uiLogViewWin()
{
    detachAllNotifiers();
}


void uiLogViewWin::uiInitCB( CallBacker* cb )
{
    uiLogViewWinBase::uiInitCB( cb );
    mAttachCB( logviewtree_->logChecked, uiLogViewWin::addLogCB );
    mAttachCB( logviewtree_->logUnchecked, uiLogViewWin::removeLogCB );
    mAttachCB( logviewtree_->markerChecked, uiLogViewWin::addMarkerCB );
    mAttachCB( logviewtree_->markerUnchecked, uiLogViewWin::removeMarkerCB );
}


void uiLogViewWin::addApplicationToolBar()
{
    tb_->addButton( addbuttonitem_ );
    tb_->addButton( rmvbuttonitem_ );
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
    uiLogViewWinBase::clearAll();
    logviewtree_->uncheckAll();
    logviewtree_->collapseAll();
}


void uiLogViewWin::addWell( const DBKey& wellkey, const TypeSet<int>& logs )
{
    addTrackCB( nullptr );
    const int vwidx = logviewtbl_->currentView();
    addWell( vwidx, wellkey, logs );
}


void uiLogViewWin::addWell( int vwidx, const DBKey& wellkey,
			    const TypeSet<int>& logs )
{
    if ( !logviewtbl_->validIdx(vwidx) )
	return;

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

	addWell( wellkey, logs );
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
	uiLogChart* chart = logviewtbl_->getCurrentLogChart();
	zdomainfld_->setCurrentItem( chart->zType() );
	logviewtbl_->clearSelection();
	logviewtbl_->updatePrimaryZrangeCB( nullptr );
	needsave_ = false;
    }
}


void uiLogViewWin::saveFile( const char* nm )
{
    const int nrviews = logviewtbl_->size();
    IOPar iop;
    iop.set( sKey::NrItems(), nrviews );

    for ( int idx=0; idx<nrviews; idx++ )
    {
	IOPar tmp;
	logviewtbl_->getLogChart(idx)->fillPar( tmp );
	iop.mergeComp( tmp, IOPar::compKey(sKey::ID(),idx) );
    }

    iop.write( nm, "Log Display" );
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
