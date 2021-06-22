if(WITH_TOOLS)
	project(sim_fire_spreading)

	# blacklist some unused files
	file(GLOB SAGA_TOOL_BLACKLIST
		SimulateVariableWind.cpp
		SimulateVariableWind.h)
	message("blacklist: ${SAGA_TOOL_BLACKLIST}")

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
