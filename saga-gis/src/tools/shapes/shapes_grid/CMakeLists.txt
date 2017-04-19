option(WITH_TOOL_SHAPES_GRID "Check to install tool shapes_grid" OFF)

if (WITH_TOOL_SHAPES_GRID OR WITH_ALL_TOOLS)
	project(shapes_grid)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
