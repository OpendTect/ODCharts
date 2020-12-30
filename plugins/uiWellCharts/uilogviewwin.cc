/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		August 2020
________________________________________________________________________

-*/


#include "uilogviewwin.h"
#include "uilogview.h"

#include "uiscrollarea.h"
#include "uitoolbutton.h"
#include "uiwellinpgrp.h"

#include "welldata.h"
#include "wellman.h"

uiLogViewWin::uiLogViewWin( uiParent* p )
    : uiMainWin(p,toUiString("OpendTect - Log Viewer"))
{
    inputgrp_ = new uiWellInputGroup( this );

    auto* addbut = new uiToolButton( this, uiToolButton::RightArrow,
				toUiString("Add"),
				mCB(this,uiLogViewWin,addCB) );
    addbut->attach( centeredRightOf, inputgrp_ );

//    auto* sa = new uiScrollArea( this, "Viewer Area" );
//    sa->attach( ensureRightOf, addbut );
    viewgrp_ = new uiGroup( this, "Viewer Group" );
    viewgrp_->attach( ensureRightOf, addbut );
//    sa->setObject( viewgrp_->attachObj() );

    addViewer();
}


uiLogViewWin::~uiLogViewWin()
{
    detachAllNotifiers();
}


void uiLogViewWin::addCB( CallBacker* )
{
    TypeSet<MultiID> wellkeys;
    BufferStringSet lognms;
    inputgrp_->getSelection( wellkeys, lognms );

    for ( int widx=0; widx<wellkeys.size(); widx++ )
    {
	RefMan<Well::Data> wd =
		Well::MGR().get( wellkeys.get(widx), Well::Logs );
	for ( int lidx=0; lidx<lognms.size(); lidx++ )
	{
	    logviews_[0]->addLog( *wd, lognms.get(lidx) );
	}
    }
}


void uiLogViewWin::addViewer()
{
    auto* vwr = new uiLogView( viewgrp_, "viewer 1" );
    vwr->setStretch( 2, 2 );
    if ( !logviews_.isEmpty() )
	vwr->attach( rightOf, logviews_.last() );

    logviews_ += vwr;
}
