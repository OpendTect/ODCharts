#
#	CMake : 	uiCharts
# 	Date:		November 2020
#	Author:		Nanne Hemstra
#_______________________________________________________________________________

SET( OD_MODULE_DEPS uiCharts uiTools Well )
SET( OD_IS_PLUGIN yes )
SET( OD_MODULE_SOURCES
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
	wellchartcommon.cc
)

set( OD_MODULE_PROGS od_LogViewer.cc )

SET( OD_PLUGIN_ALO_EXEC ${OD_MAIN_EXEC} )
OD_INIT_MODULE()
