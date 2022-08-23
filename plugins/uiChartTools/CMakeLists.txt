#________________________________________________________________________
#
# Copyright:	(C) 1995-2022 dGB Beheer B.V.
# License:	https://dgbes.com/licensing
#________________________________________________________________________
#

SET( OD_MODULE_DEPS uiCharts uiTools uiWellCharts )
SET( OD_IS_PLUGIN yes )
SET( OD_MODULE_SOURCES
	uicharttoolspi.cc
	uichartsfunctiondisplay.cc
	uichartsfunctiondisplayserver.cc
	uiwellchartsdisplayserver.cc
)

SET( OD_PLUGIN_ALO_EXEC ${OD_MAIN_EXEC} )
OD_INIT_MODULE()
