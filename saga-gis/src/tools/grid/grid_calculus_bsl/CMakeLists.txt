option(WITH_TOOL_GRID_CALCULUS_BSL "Check to install tool grid_calculus_bsl" OFF)

if (WITH_TOOL_GRID_CALCULUS_BSL OR WITH_ALL_TOOLS)
	project(grid_calculus_bsl)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
