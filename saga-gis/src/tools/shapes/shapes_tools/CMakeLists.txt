option(WITH_TOOL_SHAPES_TOOLS "Check to install tool shapes_tools" OFF)

if (WITH_TOOL_SHAPES_TOOLS OR WITH_ALL_TOOLS)
	project(shapes_tools)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
