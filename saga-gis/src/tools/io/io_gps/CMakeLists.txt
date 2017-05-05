option(WITH_TOOL_IO_GPS "Check to install tool io_gps" OFF)

if (WITH_TOOL_IO_GPS OR WITH_ALL_STANDARD_TOOLS OR WITH_ALL_TOOLS)
	project(io_gps)

	# load common tool template
	include(../../CMakePluginTemplate.cmake)
endif()
