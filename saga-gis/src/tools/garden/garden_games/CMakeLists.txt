option(WITH_TOOL_GARDEN_GAMES "Check to install tool garden_games" OFF)

if (WITH_TOOL_GARDEN_GAMES OR WITH_ALL_TOOLS)
	project(garden_games)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
