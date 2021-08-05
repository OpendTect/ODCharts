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
#include "uilogviewtable.h"
#include "uimsg.h"
#include "uiscrollarea.h"
#include "uisplitter.h"
#include "uitable.h"
#include "uitoolbar.h"
#include "uitoolbutton.h"
#include "uiwellinpgrp.h"

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
    , setbuttonitem_(uiStrings::sApply(),"axis-next","",
				mCB(this,uiLogViewWin,addCB),sMnuID++)
    , addbuttonitem_(uiStrings::sAdd(),"plus","",
				mCB(this,uiLogViewWin,addTrackCB),sMnuID++)
    , rmvbuttonitem_(uiStrings::sRemove(),"minus","",
			    mCB(this,uiLogViewWin,rmvTrackCB),sMnuID++)
{
    mainObject()->setMinimumHeight( sWinHeight );
    mainObject()->setMinimumWidth( sWinWidth );
    createToolBar();

    inputgrp_ = new uiWellInputGroup( this );
    inputgrp_->setStretch( 0, 2 );

    logviewtbl_ = new uiLogViewTable( this, true );

    auto* splitter = new uiSplitter( this );
    splitter->addGroup( inputgrp_ );
    splitter->addGroup( logviewtbl_ );

    mAttachCB( postFinalise(), uiLogViewWin::uiInitCB );
}


uiLogViewWin::~uiLogViewWin()
{
    detachAllNotifiers();
}


void uiLogViewWin::uiInitCB( CallBacker* )
{
    mAttachCB( windowClosed, uiLogViewWin::closeCB );
}


void uiLogViewWin::createToolBar()
{
    tb_ = new uiToolBar( this, tr("Log Viewer") );
    tb_->addButton( newitem_ );
    tb_->addButton( openitem_ );
    tb_->addButton( saveitem_ );
    tb_->addButton( saveasitem_ );
    tb_->addSeparator();
    tb_->addButton( setbuttonitem_ );
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
    {
	if ( chart->hasLogCurve(wellkey,*lognm) )
	    continue;

	chart->addLogCurve( wellkey, *lognm );
    }

    Well::LoadReqs lreq( Well::Trck );
    RefMan<Well::Data> wd = Well::MGR().get( wellkey, lreq );
    logviewtbl_->addToViewLabel( idx, wd->name() );
    if ( logviewtbl_->masterZRange().includes(wd->track().zRange()) )
	chart->getZAxis()->setAxisLimits( logviewtbl_->masterZRange() );
    else
	logviewtbl_->updateMasterZrangeCB( nullptr );

    needsave_ = true;
}


void uiLogViewWin::addCB( CallBacker* )
{
    TypeSet<MultiID> wellkeys;
    BufferStringSet lognms;
    inputgrp_->getSelection( wellkeys, lognms );

    if ( logviewtbl_->isEmpty() )
	logviewtbl_->addTrackCB( nullptr );

    uiLogChart* chart = logviewtbl_->getCurrentLogChart();
    if ( !chart )
	return;

    chart->removeAllCurves();
    logviewtbl_->setViewLabel( logviewtbl_->currentView(), uiString::empty() );
    for ( int widx=0; widx<wellkeys.size(); widx++ )
	add( logviewtbl_->currentView(), wellkeys[widx], lognms );

}


void uiLogViewWin::clearAll()
{
    logviewtbl_->setEmpty();
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

    clearAll();
    const BufferString defseldir =
	FilePath( GetDataDir() ).add( defDirStr() ).fullPath();
    uiFileDialog dlg( this, true, 0, filtStr(),
		      tr("Load Log View parameters") );
    dlg.setDirectory(defseldir);
    if (!dlg.go())
	return;

    filename_ = dlg.fileName();
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
	    logviewtbl_->setViewLabel( logviewtbl_->currentView(),
				    toUiString(chart->wellNames().cat(" ")) );
	}
	logviewtbl_->clearSelection();
	logviewtbl_->updateMasterZrangeCB( nullptr );
	needsave_ = false;
    }
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
    needsave_ = true;
}


void uiLogViewWin::closeCB( CallBacker* )
{
   checkSave();
}
