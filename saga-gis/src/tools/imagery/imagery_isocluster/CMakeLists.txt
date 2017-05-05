option(WITH_TOOL_IMAGERY_ISOCLUSTER "Check to install tool imagery_isocluster" OFF)

if (WITH_TOOL_IMAGERY_ISOCLUSTER OR WITH_ALL_STANDARD_TOOLS OR WITH_ALL_TOOLS)
	project(imagery_isocluster)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
