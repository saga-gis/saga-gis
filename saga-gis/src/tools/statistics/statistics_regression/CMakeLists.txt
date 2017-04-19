option(WITH_TOOL_STATISTICS_REGRESSION "Check to install tool statistics_regression" OFF)

if (WITH_TOOL_STATISTICS_REGRESSION OR WITH_ALL_TOOLS)
	project(statistics_regression)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
