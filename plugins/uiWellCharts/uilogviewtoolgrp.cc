/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		August 2021
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
    uiToolButton* tool = new uiToolButton( this, tbs );
    if ( toggle )
	tool->setOn( false );
    if ( id != -1 )
	tool->setID( id );
    if ( !addedobjects_.isEmpty() )
	tool->attach( rightOf, addedobjects_.last() );
    addedobjects_ += tool;
    return tool->id();
}


void uiLogViewToolGrp::addObject( uiObject* obj )
{
    if ( obj )
    {
	mDynamicCastGet(uiToolButton*,button,obj)
	if ( !button )
	    obj->setMaximumHeight( uiObject::iconSize() );
	obj->reParent( this );
	if ( !addedobjects_.isEmpty() )
	    obj->attach( rightOf, addedobjects_.last() );
	addedobjects_ += obj;
    }
}


bool uiLogViewToolGrp::isLocked() const
{
    mDynamicCastGet(const uiToolButton*,tool,addedobjects_.first());
    if ( tool )
	return tool->isOn();

    return false;
}


void uiLogViewToolGrp::lockCB( CallBacker* )
{
    mDynamicCastGet(uiToolButton*,tool,addedobjects_.first());
    if ( tool )
	tool->setIcon( tool->isOn() ? "lock" : "unlock" );
}
