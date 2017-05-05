option(WITH_TOOL_GRID_SPLINE "Check to install tool grid_spline" OFF)

if (WITH_TOOL_GRID_SPLINE OR WITH_ALL_STANDARD_TOOLS OR WITH_ALL_TOOLS)
	project(grid_spline)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
