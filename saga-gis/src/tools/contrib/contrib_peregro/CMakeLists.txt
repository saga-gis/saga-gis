option(WITH_TOOL_CONTRIB_PEREGRO "Check to install tool contrib_peregro" OFF)

if (WITH_TOOL_CONTRIB_PEREGRO OR WITH_ALL_TOOLS)
	project(contrib_peregro)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
