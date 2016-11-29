option(WITH_TOOL_IMAGERY_MAXENT "Check to install tool imagery_maxent" OFF)

if (WITH_TOOL_IMAGERY_MAXENT OR WITH_ALL_TOOLS)
	project(imagery_maxent)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
