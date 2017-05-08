option(WITH_TOOL_GARDEN_LEARN_TO_PROGRAM "Check to install tool garden_learn_to_program" OFF)

if (WITH_TOOL_GARDEN_LEARN_TO_PROGRAM OR WITH_ALL_STANDARD_TOOLS OR WITH_ALL_TOOLS)
	project(garden_learn_to_program)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
