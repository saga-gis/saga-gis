option(WITH_TOOL_SHAPES_TRANSECT "Check to install tool shapes_transect" OFF)

if (WITH_TOOL_SHAPES_TRANSECT OR WITH_ALL_STANDARD_TOOLS OR WITH_ALL_TOOLS)
	project(shapes_transect)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
