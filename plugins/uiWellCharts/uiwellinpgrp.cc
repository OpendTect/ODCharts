/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		August 2020
________________________________________________________________________

-*/


#include "uiwellinpgrp.h"

#include "uilistbox.h"

#include "wellman.h"


uiWellInputGroup::uiWellInputGroup( uiParent* p )
    : uiGroup(p,"Well Input Group")
{
    wellsfld_ = new uiListBox( this, "Wells", OD::ChooseAtLeastOne );
    mAttachCB( wellsfld_->selectionChanged, uiWellInputGroup::wellSelChg );
    mAttachCB( wellsfld_->itemChosen, uiWellInputGroup::wellSelChg );
    logsfld_ = new uiListBox( this, "Logs", OD::ChooseAtLeastOne );
    logsfld_->attach( alignedBelow, wellsfld_ );

    init();
}


uiWellInputGroup::~uiWellInputGroup()
{
    detachAllNotifiers();
}


void uiWellInputGroup::init()
{
    BufferStringSet wellnms;
    Well::MGR().getWellNames( wellnms );
    wellkeys_.erase();
    Well::MGR().getWellKeys( wellkeys_ );
    wellsfld_->addItems( wellnms );
}


void uiWellInputGroup::wellSelChg( CallBacker* )
{
    logsfld_->setEmpty();

    TypeSet<int> selidxs;
    wellsfld_->getChosen( selidxs );
    if ( selidxs.isEmpty() )
	return;

    BufferStringSet alllognms;
    for ( int idx=0; idx<selidxs.size(); idx++ )
    {
	const MultiID& key = wellkeys_.get( selidxs[idx] );
	BufferStringSet lognms;
	Well::MGR().getLogNamesByID( key, lognms );
	if ( idx == 0 )
	    alllognms = lognms;
	else
	{
	    for ( int lidx=alllognms.size()-1; lidx>=0; lidx-- )
	    {
		if (!lognms.isPresent(alllognms.get(lidx)) )
		    alllognms.removeSingle( lidx );
	    }
	}
    }

    alllognms.sort();
    logsfld_->addItems( alllognms );
}


void uiWellInputGroup::getSelection( TypeSet<MultiID>& keys,
				     BufferStringSet& lognms ) const
{
    TypeSet<int> selidxs;
    wellsfld_->getChosen( selidxs );
    for ( int idx=0; idx<selidxs.size(); idx++ )
	keys += wellkeys_.get( selidxs[idx] );

    logsfld_->getChosen( lognms );
}
