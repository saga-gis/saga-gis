option(WITH_TOOL_PJ_GEOREFERENCE "Check to install tool pj_georeference" OFF)

if (WITH_TOOL_PJ_GEOREFERENCE OR WITH_ALL_STANDARD_TOOLS OR WITH_ALL_TOOLS)
	project(pj_georeference)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
