option(WITH_TOOL_TA_COMPOUND "Check to install tool ta_compound" OFF)

if (WITH_TOOL_TA_COMPOUND OR WITH_ALL_TOOLS)
	project(ta_compound)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
