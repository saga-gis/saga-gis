option(WITH_TOOL_SIM_AIR_FLOW "Check to install tool sim_air_flow" OFF)

if (WITH_TOOL_SIM_AIR_FLOW OR WITH_ALL_STANDARD_TOOLS OR WITH_ALL_TOOLS)
	project(sim_air_flow)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
