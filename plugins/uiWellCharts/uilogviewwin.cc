/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		November 2020
________________________________________________________________________

-*/

#include "uilogviewwin.h"

#include "logcurve.h"
#include "markerline.h"
#include "uibutton.h"
#include "uichartaxes.h"
#include "uicombobox.h"
#include "uifiledlg.h"
#include "uilabel.h"
#include "uilogchart.h"
#include "uilogview.h"
#include "uilogviewpropdlg.h"
#include "uilogviewertree.h"
#include "uilogviewtable.h"
#include "uimain.h"
#include "uimsg.h"
#include "uiscrollarea.h"
#include "uispinbox.h"
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
    setDeleteOnClose( true );

    logviewtbl_ = new uiLogViewTable( this, nrcol, showtools );

    mAttachCB( postFinalize(), uiLogViewWinBase::uiInitCB );
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

    EnumDef def = uiWellCharts::ZTypeDef();
    if ( !SI().zIsTime() )
	def.remove( def.getKeyForIndex(uiWellCharts::TWT) );

    auto* lbl = new uiLabel( nullptr, tr("Z Domain") );
    tb_->addObject( lbl );
    zdomainfld_ = new uiComboBox( nullptr, def, nullptr );
    mAttachCB( zdomainfld_->selectionChanged, uiLogViewWinBase::zdomainChgCB );
    tb_->addObject( zdomainfld_ );

    addApplicationToolBar();
    tb_->addSeparator();
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


void uiLogViewWinBase::loadFile( const char* nm )
{
    clearAll();

    filename_ = nm;
    IOPar iop;
    if ( !iop.read(filename_,"Log Display",true) )
	return;

    int nitems = 0;
    iop.get( sKey::NrItems(), nitems );
    if ( nitems<1 )
	return;

    for ( int idx=nitems-1; idx>=0; idx-- )
    {
	logviewtbl_->addTrackCB( nullptr );
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


void uiLogViewWinBase::saveFile( const char* nm )
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



// uiLockedLogViewWin
uiLockedLogViewWin::uiLockedLogViewWin( uiParent* p,
					const ObjectSet<Well::Data>& wds,
					const BufferStringSet& lognms,
					const BufferStringSet& markernms )
    : uiLogViewWinBase(p, 0, false)
    , unzoombuttonitem_(tr("View All Z"),"view_all","",
			    mCB(this,uiLockedLogViewWin,zoomResetCB),sMnuID++)
    , settingsbuttonitem_(uiStrings::sSettings(),"settings","",
			mCB(this,uiLockedLogViewWin,showSettingsCB),sMnuID++)
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

    mAttachCB(zdomainfld_->selectionChanged, uiLockedLogViewWin::flattenChgCB);
}


uiLockedLogViewWin::~uiLockedLogViewWin()
{
    detachAllNotifiers();
    delete propdlg_;
}


void uiLockedLogViewWin::loadFile( const char* nm )
{
    uiLogViewWinBase::loadFile( nm );
    DBKeySet wellids;
    BufferStringSet lognms, markernms;
    for ( int idx=0; idx<logviewtbl_->size(); idx++ )
    {
	auto* logchart = logviewtbl_->getLogChart( idx );
	wellids.append( logchart->wellIDs(), false );
	for ( const auto* logcurve : logchart->logcurves() )
	    lognms.addIfNew( logcurve->logName() );
	for ( const auto* marker : logchart->markers() )
	    markernms.addIfNew( marker->markerName() );
    }

    logfiltergrp_->setSelected( wellids, lognms, markernms );
    dataChgCB( nullptr );
}


void uiLockedLogViewWin::addApplicationToolBar()
{
    flattenfld_ = new uiCheckBox( nullptr, tr("Flatten on") );
    flattenfld_->setChecked( false );
    mAttachCB(flattenfld_->activated, uiLockedLogViewWin::flattenChgCB);
    tb_->addObject( flattenfld_ );

    markerfld_ = new uiComboBox( nullptr, nullptr );
    markerfld_->setSensitive( false );
    mAttachCB(markerfld_->selectionChanged, uiLockedLogViewWin::flattenChgCB);
    tb_->addObject( markerfld_ );

    auto* lbl = new uiLabel( nullptr, tr(" Z Range") );
    tb_->addObject( lbl );
    zrangetopfld_ = new uiSpinBox( nullptr, 0 );
    zrangetopfld_->setHSzPol( uiObject::Small );
    tb_->addObject( zrangetopfld_ );
    tb_->addObject( new uiLabel(nullptr, tr("-")) );
    zrangebotfld_ = new uiSpinBox( nullptr, 0 );
    zrangebotfld_->setHSzPol( uiObject::Small );
    tb_->addObject( zrangebotfld_ );
    mAttachCB(zrangetopfld_->valueChanged, uiLockedLogViewWin::zoomRangeCB);
    mAttachCB(zrangebotfld_->valueChanged, uiLockedLogViewWin::zoomRangeCB);
    mAttachCB(logviewtbl_->syncRangeChg,uiLockedLogViewWin::zoomRangeChgCB);

    tb_->addButton( unzoombuttonitem_ );
    tb_->addButton( settingsbuttonitem_ );
}


void uiLockedLogViewWin::setSelected( const DBKeySet& wellids,
				      const BufferStringSet& logs,
				      const BufferStringSet& mrkrs )
{
    addWellData( wellids, logs, mrkrs );
    markerfld_->setEmpty();
    markerfld_->addItems( mrkrs );
    logviewtbl_->setAllLocked( true );
    logfiltergrp_->setSelected( wellids, logs, mrkrs );
    const Interval<float> zrange = logviewtbl_->primaryZRange();
    zrangetopfld_->setValue( zrange.start );
    zrangebotfld_->setValue( zrange.stop );
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
    markerfld_->setEmpty();
    markerfld_->addItems( mrknms );
    logviewtbl_->setAllLocked( true );
    const Interval<float> zrange = logviewtbl_->primaryZRange();
    zrangetopfld_->setValue( zrange.start );
    zrangebotfld_->setValue( zrange.stop );
}


void uiLockedLogViewWin::flattenChgCB( CallBacker* )
{
    const bool ischecked = flattenfld_->isChecked();
    auto* curlogchart = logviewtbl_->getCurrentLogChart();
    if ( !curlogchart && ischecked )
    {
	uiMSG().message( tr("Please select a log chart") );
	flattenfld_->setChecked( false );
	markerfld_->setSensitive( false );
	zrangetopfld_->setSensitive( true );
	zrangebotfld_->setSensitive( true );
	return;
    }

    markerfld_->setSensitive( ischecked );
    zrangetopfld_->setSensitive( !ischecked );
    zrangebotfld_->setSensitive( !ischecked );
    if ( ischecked )
    {
	BufferString marker = markerfld_->text();
	float curmarkerz = curlogchart->getMarkerZ( marker );

	for ( int idx=0; idx<logviewtbl_->size(); idx++ )
	{
	    auto* lc = logviewtbl_->getLogChart( idx );
	    float zshift = 0.f;
	    if ( lc && lc!=curlogchart )
	    {
		zshift = lc->getMarkerZ( marker );
		if ( mIsUdf(zshift) )
		    zshift = 0.f;
		else
		    zshift -= curmarkerz;
	    }
	    lc->setZShift( zshift );
	}
    }
    else
    {
	for ( int idx=0; idx<logviewtbl_->size(); idx++ )
	{
	    auto* lc = logviewtbl_->getLogChart( idx );
	    if ( !lc )
		continue;
	    lc->setZShift( 0.f );
	}
    }
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


void uiLockedLogViewWin::zoomRangeCB( CallBacker* )
{
    if ( flattenfld_->isChecked() )
	return;

    const float zmin = zrangetopfld_->getFValue();
    const float zmax = zrangebotfld_->getFValue();
    if ( zmin>=zmax )
    {
	uiMSG().message( tr("Please check the z range specified") );
	return;
    }

    for ( int idx=0; idx<logviewtbl_->size(); idx++ )
    {
	uiLogChart* lc = logviewtbl_->getLogChart( idx );
	NotifyStopper ns( lc->getZAxis()->rangeChanged );
	lc->setZRange( zmin, zmax);
    }
}


void uiLockedLogViewWin::zoomRangeChgCB( CallBacker* cb )
{
    if ( flattenfld_->isChecked() )
	return;

    mCBCapsuleUnpack(const Interval<float>&,range,cb);
    NotifyStopper ns1( zrangetopfld_->valueChanged );
    NotifyStopper ns2( zrangebotfld_->valueChanged );
    zrangetopfld_->setValue( range.start );
    zrangebotfld_->setValue( range.stop );
}


void uiLockedLogViewWin::zoomResetCB( CallBacker* )
{
    for ( int idx=0; idx<logviewtbl_->size(); idx++ )
	logviewtbl_->getLogView( idx )->zoomResetCB( nullptr );
}


void uiLockedLogViewWin::zdomainChgCB( CallBacker* cb )
{
    uiLogViewWinBase::zdomainChgCB( cb );
    flattenChgCB( nullptr );
}



// uiLogViewWin
uiLogViewWin::uiLogViewWin( uiParent* p, int nrcol )
    : uiLogViewWinBase(p,nrcol)
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

    DBKeySet wellids = logchart->wellIDs();
    for ( const auto& wellid : wellids )
    {
	const BufferStringSet logs = logchart->getDispLogsForID( *wellid );
	logviewtree_->checkLogsFor( *wellid, logs );
	const BufferStringSet mrkrs = logchart->getDispMarkersForID( *wellid );
	logviewtree_->checkMarkersFor( *wellid, mrkrs );
    }
}
