option(WITH_TOOL_TABLE_TOOLS "Check to install tool table_tools" OFF)

if (WITH_TOOL_TABLE_TOOLS OR WITH_ALL_TOOLS)
	project(table_tools)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
