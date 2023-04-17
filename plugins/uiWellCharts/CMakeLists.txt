#________________________________________________________________________
#
# Copyright:	(C) 1995-2022 dGB Beheer B.V.
# License:	https://dgbes.com/licensing
#________________________________________________________________________
#

set( OD_MODULE_DEPS uiCharts uiWell )
set( CMAKE_FOLDER "OpendTect Pro" )
set( OD_IS_PLUGIN yes )
set( OD_MODULE_SOURCES
	logcurve.cc
	logdata.cc
	loggradient.cc
	markerline.cc
	uilogchart.cc
	uilogcurveprops.cc
	uilogview.cc
	uilogviewertree.cc
	uilogviewpropdlg.cc
	uilogviewtable.cc
	uilogviewtoolgrp.cc
	uilogviewwin.cc
	uiwellchartspi.cc
	wellbase.cc
	wellchartcommon.cc
)

set( OD_MODULE_PROGS od_LogViewer.cc )

OD_INIT_MODULE()
