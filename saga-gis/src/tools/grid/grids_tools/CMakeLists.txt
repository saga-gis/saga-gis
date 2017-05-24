option(WITH_TOOL_GRIDS_TOOLS "Check to install tool grids_tools" OFF)

if (WITH_TOOL_GRIDS_TOOLS OR WITH_ALL_STANDARD_TOOLS OR WITH_ALL_TOOLS)
	project(grids_tools)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
