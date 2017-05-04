option(WITH_TOOL_CLIMATE_TOOLS "Check to install tool climate_tools" OFF)

if (WITH_TOOL_CLIMATE_TOOLS OR WITH_ALL_STANDARD_TOOLS OR WITH_ALL_TOOLS)
	project(climate_tools)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
