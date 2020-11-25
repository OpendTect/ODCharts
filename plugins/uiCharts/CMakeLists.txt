#
#	CMake : 	uiCharts
# 	Date:		November 2020
#	Author:		Nanne Hemstra
#_______________________________________________________________________________

SET( OD_MODULE_DEPS uiBase )
SET( OD_IS_PLUGIN yes )
SET( OD_USEQT Charts )
SET( OD_MODULE_SOURCES
	uichartspi.cc
	uichart.cc
	uichartseries.cc
	uichartview.cc
)

SET( OD_PLUGIN_ALO_EXEC ${OD_MAIN_EXEC} )
OD_INIT_MODULE()
