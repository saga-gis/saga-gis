option(WITH_TOOL_SIM_LANDSCAPE_EVOLUTION "Check to install tool sim_landscape_evolution" OFF)

if (WITH_TOOL_SIM_LANDSCAPE_EVOLUTION OR WITH_ALL_STANDARD_TOOLS OR WITH_ALL_TOOLS)
	project(sim_landscape_evolution)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
