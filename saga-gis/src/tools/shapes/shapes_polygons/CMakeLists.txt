option(WITH_TOOL_SHAPES_POLYGONS "Check to install tool shapes_polygons" OFF)

if (WITH_TOOL_SHAPES_POLYGONS OR WITH_ALL_TOOLS)
	project(shapes_polygons)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
