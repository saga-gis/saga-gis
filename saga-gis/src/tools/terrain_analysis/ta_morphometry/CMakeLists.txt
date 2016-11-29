option(WITH_TOOL_TA_MORPHOMETRY "Check to install tool ta_morphometry" OFF)

if (WITH_TOOL_TA_MORPHOMETRY OR WITH_ALL_TOOLS)
	project(ta_morphometry)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
