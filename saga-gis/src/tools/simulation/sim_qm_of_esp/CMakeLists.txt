option(WITH_TOOL_SIM_QM_OF_ESP "Check to install tool sim_qm_of_esp" OFF)

if (WITH_TOOL_SIM_QM_OF_ESP OR WITH_ALL_TOOLS)
	project(sim_qm_of_esp)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
