/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uiwellchartsdisplayserver.h"

#include "draw.h"
#include "logcurve.h"
#include "welldata.h"
#include "welllog.h"
#include "welllogset.h"
#include "wellman.h"

#include "uichartslogdisplaygrp.h"
#include "uilogchart.h"
#include "uilogviewtable.h"
#include "uilogviewwin.h"
#include "uiwelllogtools.h"
#include "uiwelllogtoolsgrp.h"


uiWellChartsDisplayServer::uiWellChartsDisplayServer()
    : uiWellDisplayServer()
{}


uiWellChartsDisplayServer::~uiWellChartsDisplayServer()
{}


uiMainWin* uiWellChartsDisplayServer::createMultiWellDisplay( uiParent* p,
							 const DBKeySet& wells,
						 const BufferStringSet& loglst )
{
    if ( wells.isEmpty() )
	return nullptr;

    auto* logwin = new uiLogViewWin( p, wells.size() );
    logwin->setDeleteOnClose( true );
    BufferStringSet all_lognms;
    ManagedObjectSet<TypeSet<int>>logids;
    for ( int idx=0; idx<wells.size(); idx++ )
    {
	BufferStringSet sel_lognms;
	sel_lognms.unCat( all_lognms.validIdx(idx) ? loglst.get(idx).buf()
						: loglst.last()->buf(), "," );
	auto* logs = new TypeSet<int>;
	Well::MGR().getLogIDs( wells[idx], sel_lognms, *logs );
	logids += logs;
    }
    logwin->addWellData( wells, logids );
    logwin->setCurrentView( 0 );
    logwin->show();
    return logwin;
}


uiMainWin* uiWellChartsDisplayServer::createLogViewWin( uiParent* p,
			const ObjectSet<Well::Data>& wd,
			const BufferStringSet& lognms,
			const BufferStringSet& markernms,
		        const DBKeySet& sel_ids,
			const BufferStringSet& sel_lognms,
			const BufferStringSet& sel_markernms )
{
    auto* dlg = new uiLockedLogViewWin( p, wd, lognms, markernms, true );
    dlg->setSelected( sel_ids, sel_lognms, sel_markernms, false );
    dlg->setDeleteOnClose( true );
    dlg->setCurrentView( 0 );
    return dlg;
}


uiMainWin* uiWellChartsDisplayServer::createLogViewWin( uiParent* p,
			const ObjectSet<Well::Data>& wd,
			const MnemonicSelection& sel,
			const BufferStringSet& markernms,
		        const DBKeySet& sel_ids,
			const MnemonicSelection& sel_mns,
			const BufferStringSet& sel_markernms )
{
    auto* dlg = new uiLockedLogViewWin( p, wd, sel, markernms, true );
    dlg->setSelected( sel_ids, sel_mns, sel_markernms, false );
    dlg->setDeleteOnClose( true );
    dlg->setCurrentView( 0 );
    return dlg;
}


uiWellLogToolWinGrp*
	uiWellChartsDisplayServer::createWellLogToolGrp(uiParent* p,
			const ObjectSet<WellLogToolData>& logs )
{
    return new uiWellChartsLogToolWinGrp( p, logs );
}


uiLogDisplayGrp* uiWellChartsDisplayServer::createLogDisplayGrp( uiParent* p )
{
    return new uiChartsLogDisplayGrp( p );
}


static const int cPrefWidth = 500;
static const int cPrefHeight = 450;

uiWellChartsLogToolWinGrp::uiWellChartsLogToolWinGrp( uiParent* p,
			  const ObjectSet<WellLogToolData>& logs )
    :uiWellLogToolWinGrp(p, logs)
{
    int nrvw = 0;
    for ( const auto* logdata : logdatas_ )
	nrvw += logdata->inpLogs().size();

    logviewtbl_ = new uiLogViewTable( this, nrvw, true );
    logviewtbl_->attachObj()->setPrefWidth( cPrefWidth );
    logviewtbl_->attachObj()->setPrefHeight( cPrefHeight );

    logviewtbl_->setCurrentView( 0 );
}


uiWellChartsLogToolWinGrp::~uiWellChartsLogToolWinGrp()
{}


void uiWellChartsLogToolWinGrp::displayLogs()
{
    OD::LineStyle ls;
    BufferStringSet wellnms;
    ObjectSet<const Well::Log> inplogs;
    ObjectSet<const Well::Log> outplogs;
    for ( const auto* logdata : logdatas_ )
    {
	for ( const auto* log : logdata->inpLogs() )
	{
	    wellnms.add( logdata->wellName() );
	    inplogs += log;
	    outplogs += logdata->logs().getLog( log->name().buf() );
	}
    }

    uiLogChart* chart = logviewtbl_->getLogChart( 0 );
    if ( !chart )
	return;

    if ( !chart->getLogCurve(inplogs[0]->name()) )
    {
	ls.color_ = OD::Color::stdDrawColor( 0 );
	logviewtbl_->addWellData( wellnms, inplogs, ls );
    }
    else
    {
	ls.color_ = OD::Color::stdDrawColor( 1 );
	logviewtbl_->addWellData( wellnms, outplogs, ls, "_out" );
    }

    for ( int idx=0; idx<logviewtbl_->size(); idx++ )
	logviewtbl_->setViewLocked( idx, true );
}
