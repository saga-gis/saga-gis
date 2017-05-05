option(WITH_TOOL_TA_PREPROCESSOR "Check to install tool ta_preprocessor" OFF)

if (WITH_TOOL_TA_PREPROCESSOR OR WITH_ALL_STANDARD_TOOLS OR WITH_ALL_TOOLS)
	project(ta_preprocessor)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
