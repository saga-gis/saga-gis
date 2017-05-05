option(WITH_TOOL_SIM_FIRE_SPREADING "Check to install sim_fire_spreading" OFF)

if (WITH_TOOL_SIM_FIRE_SPREADING OR WITH_ALL_STANDARD_TOOLS OR WITH_ALL_TOOLS)
	project(sim_fire_spreading)


	# blacklist some unused files
	file(GLOB SAGA_TOOL_BLACKLIST
		SimulateVariableWind.cpp
		SimulateVariableWind.h)
	message("blacklist: ${SAGA_TOOL_BLACKLIST}")
	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
