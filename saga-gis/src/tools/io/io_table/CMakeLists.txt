option(WITH_TOOL_IO_TABLE "Check to install tool io_table" OFF)

if (WITH_TOOL_IO_TABLE OR WITH_ALL_TOOLS)
	project(io_table)

	# load common tool template
	include(../../CMakePluginTemplate.cmake)
endif()
