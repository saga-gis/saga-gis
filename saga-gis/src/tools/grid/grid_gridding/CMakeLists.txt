option(WITH_TOOL_GRID_GRIDDING "Check to install tool grid_gridding" OFF)
option(WITH_TOOL_GRID_GRIDDING_DISABLE_TRIANGLE
	"Check to compile without triangle.c (only free for non-commercial usage), and use qhull instead (requires qhull)" OFF)

if (WITH_TOOL_GRID_GRIDDING OR WITH_ALL_TOOLS)
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
	set(SAGA_TOOL_CUSTOM_HEADER_LIST
		grid_cell_polygon_coverage.h
		Interpolation.h
		Interpolation_AngularDistance.h
		Interpolation_InverseDistance.h
		Interpolation_NaturalNeighbour.h
		Interpolation_NearestNeighbour.h
		Interpolation_Shepard.h
		Interpolation_Triangulation.h
	)
	if(NOT WITH_TOOL_GRID_GRIDDING_DISABLE_TRIANGLE)
		# use embedded triangle.c
		list(APPEND SAGA_TOOL_CUSTOM_SOURCE_LIST
			nn/triangle.c)
		list(APPEND SAGA_TOOL_CUSTOM_HEADER_LIST
			nn/triangle.h)
	endif()
	# load common tool template
	include(../../CMakePluginTemplate.cmake )

	if(WITH_TOOL_GRID_GRIDDING_DISABLE_TRIANGLE)
		# link Qhull library
		# on Ubuntu 16.04 package libqhull-dev
		find_package(Qhull REQUIRED)
		target_compile_definitions(${PROJECT_NAME} PRIVATE -DUSE_QHULL)
		target_include_directories(${PROJECT_NAME} PRIVATE ${Qhull_INCLUDE_DIRS})
		target_link_libraries(${PROJECT_NAME} ${Qhull_LIBRARIES})
	endif()
endif()
