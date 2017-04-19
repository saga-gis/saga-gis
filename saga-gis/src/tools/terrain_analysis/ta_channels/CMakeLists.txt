option(WITH_TOOL_TA_CHANNELS "Check to install tool ta_channels" OFF)

if (WITH_TOOL_TA_CHANNELS OR WITH_ALL_TOOLS)
	project(ta_channels)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
