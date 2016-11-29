option(WITH_TOOL_IMAGERY_SEGMENTATION "Check to install tool imagery_segmentation" OFF)

if (WITH_TOOL_IMAGERY_SEGMENTATION OR WITH_ALL_TOOLS)
	project(imagery_segmentation)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
