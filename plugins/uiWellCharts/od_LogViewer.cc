/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uilogviewwin.h"

#include "uimain.h"
#include "uimsg.h"
#include "uisurvey.h"

#include "applicationdata.h"
#include "commandlineparser.h"
#include "ioman.h"
#include "moddepmgr.h"
#include "prog.h"


int mProgMainFnName( int argc, char** argv )
{
    mInitProg( OD::RunCtxt::UiProgCtxt )
    SetProgramArgs( argc, argv );
    uiMain app( argc, argv );
    app.setIcon( "welllogbased" );
    ApplicationData::setApplicationName( "OpendTect - Log Viewer" );

    OD::ModDeps().ensureLoaded( "uiTools" );

    const CommandLineParser clp( argc, argv );
    uiRetVal uirv = IOMan::setDataSource( clp );
    mIfIOMNotOK( return 1 )

    PIM().loadAuto( false );
    OD::ModDeps().ensureLoaded( "uiWell" );
    PtrMan<uiMainWin> topmw = new uiLogViewWin( nullptr );
    app.setTopLevel( topmw.ptr() );
    PIM().loadAuto( true );
    topmw->show();

    return app.exec();
}
