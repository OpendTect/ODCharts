#
#	CMake : 	uiCharts
# 	Date:		November 2020
#	Author:		Nanne Hemstra
#_______________________________________________________________________________

SET( OD_MODULE_DEPS uiBase )
SET( OD_IS_PLUGIN yes )
SET( OD_USEQT Charts )
set( QT_MOC_HEADERS
	i_qchart.h
	i_qchartaxes.h
	i_qchartseries.h
)
SET( OD_MODULE_SOURCES
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

SET( OD_PLUGIN_ALO_EXEC ${OD_MAIN_EXEC} )
OD_INIT_MODULE()
