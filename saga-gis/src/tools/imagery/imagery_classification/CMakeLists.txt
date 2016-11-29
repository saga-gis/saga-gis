option(WITH_TOOL_IMAGERY_CLASSIFICATION "Check to install tool imagery_classification" OFF)

if (WITH_TOOL_IMAGERY_CLASSIFICATION OR WITH_ALL_TOOLS)
	project(imagery_classification)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
