option(WITH_TOOL_GRID_ANALYSIS "Check to install tool grid_analysis" OFF)

if (WITH_TOOL_GRID_ANALYSIS OR WITH_ALL_TOOLS)
	project(grid_analysis)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
