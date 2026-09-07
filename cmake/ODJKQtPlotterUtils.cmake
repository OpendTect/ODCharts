#________________________________________________________________________
#
# Copyright:	(C) 1995-2026 dGB Beheer B.V.
# License:	https://dgbes.com/licensing
#________________________________________________________________________
#

include_guard( GLOBAL )

# Fetches and builds JKQtPlotter against the Qt used by OpendTect (QT_DIR).
macro( OD_FIND_JKQTPLOTTER )

    if( NOT QT_DIR )
	message( FATAL_ERROR "QT_DIR is not set; required to fetch JKQtPlotter" )
    endif()
    if( NOT QT_VERSION_MAJOR )
	message( FATAL_ERROR
		 "QT_VERSION_MAJOR is not set; required to fetch JKQtPlotter" )
    endif()

    if( NOT TARGET JKQTPlotter${QT_VERSION_MAJOR} )
	set( Qt${QT_VERSION_MAJOR}_DIR "${QT_DIR}" CACHE PATH
	     "Qt${QT_VERSION_MAJOR} config dir" FORCE )
	get_filename_component( _odcharts_qt_prefix "${QT_DIR}/../../.." ABSOLUTE )
	list( PREPEND CMAKE_PREFIX_PATH "${_odcharts_qt_prefix}" )
	unset( _odcharts_qt_prefix )

	set( JKQtPlotter_GIT_TAG "v5.0.0-beta2" CACHE STRING
	     "JKQtPlotter git tag/branch/commit to fetch" )

	set( JKQtPlotter_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE )
	set( JKQtPlotter_BUILD_TOOLS OFF CACHE BOOL "" FORCE )
	set( JKQtPlotter_BUILD_TESTS OFF CACHE BOOL "" FORCE )
	set( JKQtPlotter_BUILD_LIB_JKQTFASTPLOTTER OFF CACHE BOOL "" FORCE )
	set( JKQtPlotter_BUILD_DECORATE_LIBNAMES_WITH_BUILDTYPE OFF CACHE BOOL "" FORCE )
	# Avoid Qt PrintSupport (and thus CUPS on Unix)
	set( JKQtPlotter_BUILD_FORCE_NO_PRINTER_SUPPORT ON CACHE BOOL "" FORCE )

	# JKQtPlotter may override these; restore after FetchContent
	set( _odcharts_save_lib_outdir "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}" )
	set( _odcharts_save_runtime_outdir "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" )
	set( _odcharts_save_archive_outdir "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}" )
	set( _odcharts_save_build_shared_libs "${BUILD_SHARED_LIBS}" )
	# Static only — do not introduce a shared JKQt dependency
	set( BUILD_SHARED_LIBS OFF )

	include( FetchContent )
	FetchContent_Declare( JKQTPlotter${QT_VERSION_MAJOR}
	    GIT_REPOSITORY https://github.com/jkriege2/JKQtPlotter.git
	    GIT_TAG ${JKQtPlotter_GIT_TAG}
	    GIT_SHALLOW TRUE
	)
	FetchContent_MakeAvailable( JKQTPlotter${QT_VERSION_MAJOR} )

	set( BUILD_SHARED_LIBS "${_odcharts_save_build_shared_libs}" )
	set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${_odcharts_save_lib_outdir}" )
	set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${_odcharts_save_runtime_outdir}" )
	set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${_odcharts_save_archive_outdir}" )
	unset( _odcharts_save_build_shared_libs )
	unset( _odcharts_save_lib_outdir )
	unset( _odcharts_save_runtime_outdir )
	unset( _odcharts_save_archive_outdir )

	if( NOT TARGET JKQTPlotter${QT_VERSION_MAJOR} )
	    message( FATAL_ERROR
		     "JKQtPlotter target 'JKQTPlotter${QT_VERSION_MAJOR}' was not created" )
	endif()

	# Silence noisy warnings only when compiling JKQt itself (PRIVATE).
	# Do not mark includes as SYSTEM — plugins keep normal warning levels.
	foreach( _jkqt_base JKQTCommon JKQTMath JKQTMathText JKQTPlotter )
	    set( _jkqt_tgt "${_jkqt_base}${QT_VERSION_MAJOR}" )
	    if( NOT TARGET ${_jkqt_tgt} )
		continue()
	    endif()
	    target_compile_options( ${_jkqt_tgt} PRIVATE
		$<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wno-shadow>
		$<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wno-unused-variable>
		$<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wno-unused-but-set-variable>
		$<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wno-deprecated-declarations>
		$<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Wno-suggest-override> )
	    unset( _jkqt_tgt )
	endforeach()
	unset( _jkqt_base )
    endif()

    if( NOT TARGET ODCharts_JKQTPlotter )
	add_library( ODCharts_JKQTPlotter INTERFACE )
	target_link_libraries( ODCharts_JKQTPlotter
			       INTERFACE JKQTPlotter${QT_VERSION_MAJOR} )
    endif()
    if( NOT TARGET ODCharts::JKQTPlotter )
	add_library( ODCharts::JKQTPlotter ALIAS ODCharts_JKQTPlotter )
    endif()

endmacro( OD_FIND_JKQTPLOTTER )

macro( OD_SETUP_JKQTPLOTTER )

    OD_FIND_JKQTPLOTTER()

    if ( TARGET ODCharts::JKQTPlotter )
	list( APPEND OD_MODULE_EXTERNAL_LIBS ODCharts::JKQTPlotter )
    endif()

endmacro( OD_SETUP_JKQTPLOTTER )
