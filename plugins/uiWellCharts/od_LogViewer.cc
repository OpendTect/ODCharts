/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		December 2020
________________________________________________________________________

-*/

#include "applicationdata.h"
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
    uiMain app( argc, argv );
    ApplicationData::setApplicationName( "OpendTect - LogViewer" );

    OD::ModDeps().ensureLoaded( "uiBase" );

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
	DBKey wellid;
	wellid.fromString( wellidstr );
	if ( wellid.isUdf() || !IOM().isUsable(wellid) )
	{
	    uiMSG().error( toUiString("No valid well ID given") );
	    return 1;
	}
    }

    OD::ModDeps().ensureLoaded( "Well" );

    PtrMan<uiMainWin> logwin = new uiLogViewWin( nullptr );
    app.setTopLevel( logwin );

    PIM().loadAuto( false );
    PIM().loadAuto( true );

    logwin->show();

    return app.exec();
}
