/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		July 2021
________________________________________________________________________

-*/


#include "uilogcurveprops.h"

#include "datainpspec.h"
#include "draw.h"
#include "logcurve.h"
#include "uigeninput.h"
#include "uisellinest.h"

uiLogCurveProps::uiLogCurveProps( uiParent* p )
    : uiGroup(p)
{
    FloatInpIntervalSpec fspec;
    fspec.setName("Left", 0).setName("Right", 1);
    rangefld_ = new uiGenInput( this, tr("Display Range"), fspec );
    rangefld_->setWithCheck();

    linestylefld_ = new uiSelLineStyle( this, OD::LineStyle() );
    linestylefld_->attach( alignedBelow, rangefld_ );

    mAttachCB( rangefld_->valuechanged, uiLogCurveProps::rangeChgCB );
    mAttachCB( linestylefld_->changed, uiLogCurveProps::lineStyleChgCB );
}


uiLogCurveProps::uiLogCurveProps( uiParent* p, LogCurve* lc )
    : uiLogCurveProps(p)
{
    setLogCurve( lc );
}


uiLogCurveProps::~uiLogCurveProps()
{
    detachAllNotifiers();
}


void uiLogCurveProps::setLogCurve( LogCurve* lc )
{
    logcurve_ = lc;
    if ( logcurve_ )
    {
	rangefld_->setValue( logcurve_->dispRange() );
	linestylefld_->setStyle( logcurve_->lineStyle() );
    }
}


void uiLogCurveProps::lineStyleChgCB( CallBacker* )
{
    if ( logcurve_ )
	logcurve_->setLineStyle( linestylefld_->getStyle() );
}


void uiLogCurveProps::rangeChgCB( CallBacker* )
{
    if ( logcurve_ )
	logcurve_->setDisplayRange( rangefld_->getFInterval() );
}
