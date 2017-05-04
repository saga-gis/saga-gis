option(WITH_TOOL_SHAPES_POINTS "Check to install tool shapes_points" OFF)

if (WITH_TOOL_SHAPES_POINTS OR WITH_ALL_STANDARD_TOOLS OR WITH_ALL_TOOLS)
	project(shapes_points)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
