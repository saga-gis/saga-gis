option(WITH_TOOL_TA_CLIFFMETRICS "Check to install tool ta_cliffmetrics" OFF)

if (WITH_TOOL_TA_CLIFFMETRICS OR WITH_ALL_STANDARD_TOOLS OR WITH_ALL_TOOLS)
	project(ta_cliffmetrics)

	# add files in sudirectories
	set(SAGA_TOOL_CUSTOM_SOURCE_LIST
		CliffMetrics/2di_point.cpp
		CliffMetrics/2di_shape.cpp
		CliffMetrics/2d_point.cpp
		CliffMetrics/2d_shape.cpp
		CliffMetrics/calc_curvature.cpp
		CliffMetrics/cell.cpp
		CliffMetrics/cell_landform.cpp
		CliffMetrics/coast.cpp
		CliffMetrics/coast_landform.cpp
		CliffMetrics/coast_polygon.cpp
		CliffMetrics/create_profiles.cpp
		CliffMetrics/delineation.cpp
		CliffMetrics/gis_raster.cpp
		CliffMetrics/gis_utils.cpp
		CliffMetrics/gis_vector.cpp
		CliffMetrics/hermite_cubic.cpp
		CliffMetrics/init_grid.cpp
		CliffMetrics/i_line.cpp
		CliffMetrics/line.cpp
		CliffMetrics/locate_clifftop.cpp
		CliffMetrics/locate_coast.cpp
		CliffMetrics/multi_line.cpp
		CliffMetrics/profile.cpp
		CliffMetrics/random_numbers.cpp
		CliffMetrics/raster_grid.cpp
		CliffMetrics/read_input.cpp
		CliffMetrics/smooth_grid.cpp
		CliffMetrics/smooth_line.cpp
		CliffMetrics/utils.cpp
		CliffMetrics/utils_global.cpp
		CliffMetrics/write_output.cpp
	)

	set(SAGA_TOOL_CUSTOM_HEADER_LIST
		CliffMetrics/2di_point.h
		CliffMetrics/2di_shape.h
		CliffMetrics/2d_point.h
		CliffMetrics/2d_shape.h
		CliffMetrics/cell.h
		CliffMetrics/cell_landform.h
		CliffMetrics/cliffmetrics.h
		CliffMetrics/coast.h
		CliffMetrics/coast_landform.h
		CliffMetrics/coast_polygon.h
		CliffMetrics/delineation.h
		CliffMetrics/hermite_cubic.h
		CliffMetrics/i_line.h
		CliffMetrics/line.h
		CliffMetrics/multi_line.h
		CliffMetrics/profile.h
		CliffMetrics/raster_grid.h
	)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )

    if(UNIX)
        add_definitions(-DCPU -Dx86)
    endif()
endif()
