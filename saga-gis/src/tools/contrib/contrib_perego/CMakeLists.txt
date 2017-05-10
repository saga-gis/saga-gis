option(WITH_TOOL_CONTRIB_PEREGO "Check to install tool contrib_perego" OFF)

if (WITH_TOOL_CONTRIB_PEREGO OR WITH_ALL_STANDARD_TOOLS OR WITH_ALL_TOOLS)
	project(contrib_perego)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
