#
#	CMake : 	uiCharts
# 	Date:		November 2020
#	Author:		Nanne Hemstra
#_______________________________________________________________________________

SET( OD_MODULE_DEPS uiCharts uiTools )
SET( OD_IS_PLUGIN yes )
SET( OD_MODULE_SOURCES
	uicharttoolspi.cc
	uichartsfunctiondisplay.cc
	uichartsfunctiondisplayserver.cc
)

SET( OD_PLUGIN_ALO_EXEC ${OD_MAIN_EXEC} )
OD_INIT_MODULE()
