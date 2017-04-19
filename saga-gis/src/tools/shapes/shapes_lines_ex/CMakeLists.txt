option(WITH_TOOL_SHAPES_LINES_EX "Check to install shapes_lines_ex" OFF)

if (WITH_TOOL_SHAPES_LINES_EX OR WITH_ALL_TOOLS)
	project(shapes_lines_ex)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
