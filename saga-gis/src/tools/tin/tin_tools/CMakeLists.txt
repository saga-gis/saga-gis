option(WITH_TOOL_TIN_TOOLS "Check to install tool tin_tools" OFF)

if (WITH_TOOL_TIN_TOOLS OR WITH_ALL_TOOLS)
	project(tin_tools)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
