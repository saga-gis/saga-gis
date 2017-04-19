option(WITH_TOOL_IO_GRID "Check to install tool io_grid" OFF)

if (WITH_TOOL_IO_GRID OR WITH_ALL_TOOLS)
	project(io_grid)

	# load common tool template
	include(../../CMakePluginTemplate.cmake)
endif()
