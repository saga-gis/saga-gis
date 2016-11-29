option(WITH_TOOL_TABLE_CALCULUS "Check to install tool table_calculus" OFF)

if (WITH_TOOL_TABLE_CALCULUS OR WITH_ALL_TOOLS)
	project(table_calculus)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
