option(WITH_TOOL_SIM_ECOSYSTEMS_HUGGET "Check to install tool sim_ecosystems_hugget" OFF)

if (WITH_TOOL_SIM_ECOSYSTEMS_HUGGET OR WITH_ALL_TOOLS)
	project(sim_ecosystems_hugget)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
