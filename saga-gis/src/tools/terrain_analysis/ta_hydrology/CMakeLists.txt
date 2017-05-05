option(WITH_TOOL_TA_HYDROLOGY "Check to install tool ta_hydrology" OFF)

if (WITH_TOOL_TA_HYDROLOGY OR WITH_ALL_STANDARD_TOOLS OR WITH_ALL_TOOLS)
	project(ta_hydrology)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
