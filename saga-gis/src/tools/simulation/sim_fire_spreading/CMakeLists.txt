option(WITH_FIRE_SPREADING "Enable fire spreading tool (license requirements libfire)" ON)
if(WITH_TOOLS AND WITH_FIRE_SPREADING)
	project(sim_fire_spreading)

	file(GLOB SAGA_TOOL_BLACKLIST	# blacklist some unused files
		SimulateVariableWind.cpp
		SimulateVariableWind.h)
#	message("blacklist: ${SAGA_TOOL_BLACKLIST}")

	include(../../CMakePluginTemplate.cmake)	# load common tool template
endif()
