option(WITH_TOOL_TA_PROFILES "Check to install tool ta_profiles" OFF)

if (WITH_TOOL_TA_PROFILES OR WITH_ALL_STANDARD_TOOLS OR WITH_ALL_TOOLS)
	project(ta_profiles)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
