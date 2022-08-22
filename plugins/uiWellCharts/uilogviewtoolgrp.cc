/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uilogviewtoolgrp.h"

#include "uilogview.h"
#include "uitoolbutton.h"


// uiLogViewToolGrp
uiLogViewToolGrp::uiLogViewToolGrp( uiParent* p, uiLogView& logview )
    : uiGroup(p)
    , logview_(logview)
{
    addButton( "unlock", tr("Lock/Unlock"),
	       mCB(this,uiLogViewToolGrp,lockCB), true );
    addButton( "view_all", uiStrings::sReset(),
	       mCB(&logview,uiLogView,zoomResetCB), false );
    addButton( "settings", uiStrings::sSetup(),
	       mCB(&logview,uiLogView,showSettingsCB), false );
}


uiLogViewToolGrp::~uiLogViewToolGrp()
{
    detachAllNotifiers();
    deepErase( addedobjects_ );
}


int uiLogViewToolGrp::addButton( const char* icon, const uiString& tooltip,
				 const CallBack& cb, bool toggle, int id)
{
    uiToolButtonSetup tbs( icon, tooltip, cb );
    tbs.istoggle( toggle );
    auto* tb = new uiToolButton( this, tbs );
    if ( toggle )
	tb->setOn( false );

    if ( id != -1 )
	tb->setID( id );

    if ( !addedobjects_.isEmpty() )
	tb->attach( rightOf, addedobjects_.last() );

    addedobjects_ += tb;
    return tb->id();
}


void uiLogViewToolGrp::addObject( uiObject* obj )
{
    if ( !obj )
	return;

    mDynamicCastGet(uiToolButton*,tb,obj)
    if ( !tb )
	obj->setMaximumHeight( uiObject::iconSize() );

    obj->reParent( this );
    if ( !addedobjects_.isEmpty() )
	obj->attach( rightOf, addedobjects_.last() );

    addedobjects_ += obj;
}


bool uiLogViewToolGrp::isLocked() const
{
    mDynamicCastGet(const uiToolButton*,tb,addedobjects_.first())
    return tb ? tb->isOn() : false;
}


void uiLogViewToolGrp::setLocked( bool yn )
{
    mDynamicCastGet(uiToolButton*,tb,addedobjects_.first())
    if ( !tb )
	return;

    tb->setOn( yn );
    lockCB( nullptr );
}


void uiLogViewToolGrp::lockCB( CallBacker* )
{
    mDynamicCastGet(uiToolButton*,tb,addedobjects_.first());
    if ( tb )
	tb->setIcon( tb->isOn() ? "lock" : "unlock" );
}
