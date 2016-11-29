option(WITH_TOOL_IMAGERY_TOOLS "Check to install tool imagery_tools" OFF)

if (WITH_TOOL_IMAGERY_TOOLS OR WITH_ALL_TOOLS)
	project(imagery_tools)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
