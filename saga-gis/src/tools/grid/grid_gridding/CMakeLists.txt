if(WITH_TOOLS)
	project(grid_gridding)

	# add files in sudirectories
	set(SAGA_TOOL_CUSTOM_SOURCE_LIST
		./nn/delaunay.c
		./nn/hash.c
		./nn/istack.c
		./nn/lpi.c
		./nn/nnai.c
		./nn/nncommon.c
		./nn/nncommon-vulnerable.c
		./nn/nnpi.c
	)

	include(../../CMakePluginTemplate.cmake)	# load common tool template

	# check for qhull
	if(MSVC AND DEPS_FROM_ENVARS)
		if("${CMAKE_GENERATOR_PLATFORM}" STREQUAL "Win32")
			set(LIB4SAGA $ENV{QHULL_32})
		else()
			set(LIB4SAGA $ENV{QHULL})
		endif()
		find_path   (Qhull_INCLUDE_DIRS NAMES libqhull_r/qhull_ra.h PATHS "${LIB4SAGA}")
		find_library(Qhull_LIBRARIES    NAMES qhullstatic_r.lib     PATHS "${LIB4SAGA}")
		if(Qhull_INCLUDE_DIRS AND Qhull_LIBRARIES)
			mark_as_advanced(Qhull_INCLUDE_DIRS Qhull_LIBRARIES)
			include(FindPackageHandleStandardArgs)
			FIND_PACKAGE_HANDLE_STANDARD_ARGS(QHULL REQUIRED_VARS Qhull_LIBRARIES Qhull_INCLUDE_DIRS)
			target_compile_definitions(${PROJECT_NAME} PRIVATE -DHAVE_LIBQHULL_R_QHULL_RA_H)
		endif()
	endif()

	if(NOT QHULL_FOUND)
		find_package(Qhull QUIET) # ubuntu package libhpdf-dev
		find_file(LIBQHULL_R_HEADER NAMES libqhull_r/libqhull_r.h)
		if(LIBQHULL_R_HEADER)
			target_compile_definitions(${PROJECT_NAME} PRIVATE -DHAVE_LIBQHULL_R_QHULL_RA_H)
			message(STATUS "LIBQHULL_R header found")
		else()
			find_file(LIBQHULL_HEADER NAMES libqhull/libqhull.h)
			if(LIBQHULL_HEADER)
				target_compile_definitions(${PROJECT_NAME} PRIVATE -DHAVE_LIBQHULL_QHULL_A_H)
				message(STATUS "LIBQHULL header found")
			else()
				find_file(QHULL_HEADER NAMES qhull/qhull_a.h)
				if(LIBQHULL_HEADER)
					target_compile_definitions(${PROJECT_NAME} PRIVATE -DHAVE_QHULL_QHULL_A_H)
					message(STATUS "QHULL header found")
				endif()
			endif()
		endif()
		message(STATUS "Qhull_INCLUDE_DIRS ${Qhull_INCLUDE_DIRS}")
	endif()

	if(QHULL_FOUND)
		target_include_directories(${PROJECT_NAME} PRIVATE ${Qhull_INCLUDE_DIRS})
		target_link_libraries     (${PROJECT_NAME}         ${Qhull_LIBRARIES})
	endif()
endif()
