option(WITH_TOOL_SIM_RIVFLOW "Check to install tool sim_rivflow" OFF)

if (WITH_TOOL_SIM_RIVFLOW OR WITH_ALL_STANDARD_TOOLS OR WITH_ALL_TOOLS)
	project(sim_rivflow)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
