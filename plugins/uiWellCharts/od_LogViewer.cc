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


int main( int argc, char** argv )
{
    OD::SetRunContext( OD::UiProgCtxt );
    SetProgramArgs( argc, argv );
    uiMain app( argc, argv );

    PIM().loadAuto( false );
    OD::ModDeps().ensureLoaded( "Well" );
    PIM().loadAuto( true );

    CommandLineParser clp( argc, argv );
    const uiRetVal res = IOM().setDataSource( clp );
    if ( !res.isOK() )
    {
	uiMSG().error( res );
	ExitProgram( 1 );
    }

    const BufferString wellidstr = clp.keyedString("well");
    if ( !wellidstr.isEmpty() )
    {
	const DBKey wellid( wellidstr );
	if ( wellid.isUdf() || !IOM().isUsable(wellid) )
	{
	    uiMSG().error( toUiString("No valid well ID given") );
	    ExitProgram( 1 );
	}
    }

    auto* logwin = new uiLogViewWin( nullptr );
    app.setTopLevel( logwin );
    logwin->show();

    const int ret = app.exec();
    delete logwin;
    return ExitProgram( ret );
}
