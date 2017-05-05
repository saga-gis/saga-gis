option(WITH_TOOL_STATISTICS_POINTS "Check to install tool statistics_points" OFF)

if (WITH_TOOL_STATISTICS_POINTS OR WITH_ALL_STANDARD_TOOLS OR WITH_ALL_TOOLS)
	project(statistics_points)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
