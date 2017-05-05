option(WITH_TOOL_IMAGERY_PHOTOGRAMMETRY "Check to install tool imagery_photogrammetry" OFF)

if (WITH_TOOL_IMAGERY_PHOTOGRAMMETRY OR WITH_ALL_STANDARD_TOOLS OR WITH_ALL_TOOLS)
	project(imagery_photogrammetry)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
