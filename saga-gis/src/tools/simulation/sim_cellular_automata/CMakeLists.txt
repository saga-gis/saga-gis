option(WITH_TOOL_SIM_CELLULAR_AUTOMATA "Check to install tool sim_cellular_automata" OFF)

if (WITH_TOOL_SIM_CELLULAR_AUTOMATA OR WITH_ALL_STANDARD_TOOLS OR WITH_ALL_TOOLS)
	project(sim_cellular_automata)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
