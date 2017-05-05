option(WITH_TOOL_SHAPES_LINES "Check to install tool shapes_lines" OFF)

if (WITH_TOOL_SHAPES_LINES OR WITH_ALL_STANDARD_TOOLS OR WITH_ALL_TOOLS)
	project(shapes_lines)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
