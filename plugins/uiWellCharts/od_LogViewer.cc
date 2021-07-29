/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		December 2020
________________________________________________________________________

-*/

#include "uimain.h"
#include "uilogviewwin.h"

#include "applicationdata.h"
#include "commandlineparser.h"
#include "ioman.h"
#include "moddepmgr.h"
#include "prog.h"
#include "uimsg.h"



int mProgMainFnName( int argc, char** argv )
{
    mInitProg( OD::UiProgCtxt )
    SetProgramArgs( argc, argv );
    uiMain app( argc, argv );
    ApplicationData::setApplicationName( "OpendTect - Log Viewer" );
    app.setIcon( "welllogbased" );

    OD::ModDeps().ensureLoaded( "uiBase" );
    OD::ModDeps().ensureLoaded( "Well" );

    CommandLineParser clp( argc, argv );
    const uiRetVal res = IOM().setDataSource( clp );
    if ( !res.isOK() )
    {
	uiMSG().error( res );
	return 1;
    }

    PtrMan<uiMainWin> dlg = new uiLogViewWin( nullptr );

    PIM().loadAuto( false );
    PIM().loadAuto( true );

    app.setTopLevel( dlg );
    dlg->show();
    return app.exec();
}
