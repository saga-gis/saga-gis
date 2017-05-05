option(WITH_TOOL_GRID_TOOLS "Check to install tool grid_tools" OFF)

if (WITH_TOOL_GRID_TOOLS OR WITH_ALL_STANDARD_TOOLS OR WITH_ALL_TOOLS)
	project(grid_tools)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
