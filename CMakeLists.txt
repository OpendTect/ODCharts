#________________________________________________________________________
#
# Copyright:	(C) 1995-2022 dGB Beheer B.V.
# License:	https://dgbes.com/licensing
#________________________________________________________________________
#

cmake_minimum_required (VERSION 3.14)
project(ODCharts)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set ( OD_SUBSYSTEM "ODCharts" )
set ( OpendTect_DIR "" CACHE PATH "OpendTect directory" )

if( NOT DEFINED OpendTect_DIR OR NOT OpendTect_DIR OR
     "${OpendTect_DIR}" STREQUAL "" )
    get_filename_component( PARENT_DIR "${CMAKE_SOURCE_DIR}" DIRECTORY )
    get_filename_component( GRANDPARENT_DIR "${PARENT_DIR}" DIRECTORY )
    set( OpendTect_DIR "${GRANDPARENT_DIR}" )
    message( STATUS "Set OpendTect_DIR to: ${OpendTect_DIR}" )
endif()

#Find OpendTect
list(APPEND CMAKE_MODULE_PATH "${OpendTect_DIR}/CMakeModules")
find_package( OpendTect )

#Setup the modules
set( PLUGINS
    uiCharts
    uiWellCharts
    uiChartTools
)

OD_ADD_MODULES( plugins ${PLUGINS} )
