option(WITH_TOOL_GRID_VISUALISATION "Check to install tool grid_visualisation" OFF)

if (WITH_TOOL_GRID_VISUALISATION OR WITH_ALL_STANDARD_TOOLS OR WITH_ALL_TOOLS)
	project(grid_visualisation)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
