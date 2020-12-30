#
#	CMake : 	uiCharts
# 	Date:		November 2020
#	Author:		Nanne Hemstra
#_______________________________________________________________________________

SET( OD_MODULE_DEPS uiCharts Well )
SET( OD_IS_PLUGIN yes )
SET( OD_MODULE_SOURCES
	logseries.cc
	uilogview.cc
	uilogviewwin.cc
	uiwellchartspi.cc
	uiwellinpgrp.cc
)

set( OD_MODULE_PROGS od_LogViewer.cc )

SET( OD_PLUGIN_ALO_EXEC ${OD_MAIN_EXEC} )
OD_INIT_MODULE()
