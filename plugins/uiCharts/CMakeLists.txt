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
set( QT_MOC_HEADERS
	i_qchart.h
	i_qchartaxes.h
	i_qchartseries.h
)
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

OD_INIT_MODULE()
