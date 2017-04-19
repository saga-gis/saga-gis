option(WITH_TOOL_SIM_IHACRES "Check to install tool sim_ihacres" OFF)

if (WITH_TOOL_SIM_IHACRES OR WITH_ALL_TOOLS)
	project(sim_ihacres)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
