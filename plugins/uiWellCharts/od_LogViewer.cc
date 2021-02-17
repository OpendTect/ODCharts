/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		December 2019
________________________________________________________________________

-*/

#include "commandlineparser.h"
#include "ioman.h"
#include "moddepmgr.h"
#include "prog.h"

#include "uilogviewwin.h"
#include "uimainwin.h"
#include "uimain.h"
#include "uimsg.h"


int doMain( int argc, char** argv )
{
    OD::SetRunContext( OD::UiProgCtxt );
    SetProgramArgs( argc, argv );

    PIM().loadAuto( false );
    OD::ModDeps().ensureLoaded( "Well" );
    PIM().loadAuto( true );

    CommandLineParser clp( argc, argv );
    const uiRetVal res = IOM().setDataSource( clp );
    if ( !res.isOK() )
    {
	uiMSG().error( res );
	return 1;
    }

    const BufferString wellidstr = clp.keyedString("well");
    if ( !wellidstr.isEmpty() )
    {
	const DBKey wellid( wellidstr );
	if ( wellid.isUdf() || !IOM().isUsable(wellid) )
	{
	    uiMSG().error( toUiString("No valid well ID given") );
	    return 1;
	}
    }

    uiMain app( argc, argv );

    PtrMan<uiMainWin> logwin = new uiLogViewWin( nullptr );
    app.setTopLevel( logwin );
    logwin->show();

    return app.exec();
}
