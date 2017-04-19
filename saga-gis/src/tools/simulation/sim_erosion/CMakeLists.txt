option(WITH_TOOL_SIM_EROSION "Check to install tool sim_erosion" OFF)

if (WITH_TOOL_SIM_EROSION OR WITH_ALL_TOOLS)
	project(sim_erosion)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
