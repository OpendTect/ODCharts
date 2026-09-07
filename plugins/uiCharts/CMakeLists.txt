#________________________________________________________________________
#
# Copyright:	(C) 1995-2022 dGB Beheer B.V.
# License:	https://dgbes.com/licensing
#________________________________________________________________________
#

set( OD_MODULE_DEPS uiBase )
set( CMAKE_FOLDER "OpendTect Pro" )
set( OD_IS_PLUGIN yes )
set( OD_USEQT Charts )
set( OD_MODULE_SOURCES
	uicallout.cc
	uichartfillx.cc
	uichartlabel.cc
	uichartspi.cc
	uichart.cc
	uichartaxes.cc
	uichartseries.cc
	uichartview.cc
	chartutils.cc
)

if ( BUILD_JKQtPlotter )
    include( "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/ODJKQtPlotterUtils.cmake" )
    OD_SETUP_JKQTPLOTTER()
endif()

OD_INIT_MODULE()
