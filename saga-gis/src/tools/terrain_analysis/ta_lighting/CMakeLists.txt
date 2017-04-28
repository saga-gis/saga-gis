option(WITH_TOOL_TA_LIGHTING "Check to install tool ta_lighting" OFF)

if (WITH_TOOL_TA_LIGHTING OR WITH_ALL_TOOLS)
	project(ta_lighting)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
