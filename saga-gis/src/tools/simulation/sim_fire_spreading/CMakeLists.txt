if(WITH_TOOLS)
	project(sim_fire_spreading)

	file(GLOB SAGA_TOOL_BLACKLIST	# blacklist some unused files
		SimulateVariableWind.cpp
		SimulateVariableWind.h)
#	message("blacklist: ${SAGA_TOOL_BLACKLIST}")

	include(../../CMakePluginTemplate.cmake)	# load common tool template
endif()
