option(WITH_TOOL_TA_SLOPE_STABILITY "Check to install tool ta_slope_stability" OFF)

if (WITH_TOOL_TA_SLOPE_STABILITY OR WITH_ALL_TOOLS)
	project(ta_slope_stability)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
