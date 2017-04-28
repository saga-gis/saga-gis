option(WITH_TOOL_STATISTICS_GRID "Check to install tool statistics_grid" OFF)

if (WITH_TOOL_STATISTICS_GRID OR WITH_ALL_TOOLS)
	project(statistics_grid)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
