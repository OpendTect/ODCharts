#pragma once

/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		August 2020
________________________________________________________________________

-*/

#include "uiwellchartsmod.h"
#include "uigroup.h"

class uiListBox;

mExpClass(uiWellCharts) uiWellInputGroup : public uiGroup
{
public:
			uiWellInputGroup(uiParent*);
			~uiWellInputGroup();

    void		getSelection(TypeSet<MultiID>& keys,
				     BufferStringSet& lognms) const;

protected:

    uiListBox*		wellsfld_;
    uiListBox*		logsfld_;

    void		init();
    void		wellSelChg(CallBacker*);

    TypeSet<MultiID>	wellkeys_;
};

