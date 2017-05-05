option(WITH_TOOL_GARDEN_FRACTALS "Check to install tool garden_fractals" OFF)

if (WITH_TOOL_FRACTALS OR WITH_ALL_STANDARD_TOOLS OR WITH_ALL_TOOLS)
	project(garden_fractals)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
