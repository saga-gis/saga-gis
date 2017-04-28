option(WITH_TOOL_SIM_HYDROLOGY "Check to install tool sim_hydrology" OFF)

if (WITH_TOOL_SIM_HYDROLOGY OR WITH_ALL_TOOLS)
	project(sim_hydrology)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
