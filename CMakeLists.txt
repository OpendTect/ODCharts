#_______________________CMake___________________________________________________
#
#	Makefile : 	ODCharts plugin
# 	Dec 2020	Nanne Hemstra
#_______________________________________________________________________________


cmake_minimum_required (VERSION 3.14)
project(ODCharts)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set ( OD_SUBSYSTEM "ODCharts" )
set ( OpendTect_DIR "" CACHE PATH "OpendTect directory" )

if( (NOT DEFINED OpendTect_DIR) OR (NOT OpendTect_DIR) OR
     ("${OpendTect_DIR}" STREQUAL "") )
    get_filename_component( PARENT_DIR "${CMAKE_SOURCE_DIR}" DIRECTORY )
    get_filename_component( GRANDPARENT_DIR "${PARENT_DIR}" DIRECTORY )
    set( OpendTect_DIR "${GRANDPARENT_DIR}" )
    message( STATUS "Set OpendTect_DIR to: ${OpendTect_DIR}" )
endif()

#Setup output directory
option ( OD_BUILD_LOCAL "Build in local directory" ON )

#Find OpendTect
list(APPEND CMAKE_MODULE_PATH "${OpendTect_DIR}/CMakeModules")
find_package( OpendTect )

#Setup the modules
set ( PLUGINS  uiCharts uiWellCharts )
OD_ADD_MODULES( plugins ${PLUGINS} )

#Add the plugins
foreach ( plugin ${PLUGINS} )
    list( APPEND RUNTIMEPATH ${OD_${plugin}_RUNTIMEPATH} )
endforeach()

list ( APPEND RUNTIMEPATH ${OD_BINARY_BASEDIR}/${OD_EXEC_OUTPUT_RELPATH} )
list ( REMOVE_DUPLICATES RUNTIMEPATH )
