#________________________________________________________________________
#
# Copyright:	(C) 1995-2022 dGB Beheer B.V.
# License:	https://dgbes.com/licensing
#________________________________________________________________________
#

set( OD_MODULE_DEPS uiWellCharts )
set( CMAKE_FOLDER "OpendTect Pro" )
set( OD_IS_PLUGIN yes )
set( OD_MODULE_SOURCES
	uicharttoolspi.cc
	uichartsfunctiondisplay.cc
	uichartsfunctiondisplayserver.cc
	uichartslogdisplaygrp.cc
	uiwellchartsdisplayserver.cc
)

set( OD_PLUGIN_ALO_EXEC ${OD_MAIN_EXEC} )

OD_INIT_MODULE()
