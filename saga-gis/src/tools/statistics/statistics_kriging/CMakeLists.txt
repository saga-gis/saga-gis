option(WITH_TOOL_STATISTICS_KRIGING "Check to install tool statistics_kriging" OFF)

if (WITH_TOOL_STATISTICS_KRIGING OR WITH_ALL_TOOLS)
	project(statistics_kriging)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )

	# include directories of wxWidgets are already present
	find_package(wxWidgets REQUIRED)
	target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})

	# link saga_gdi
	target_link_libraries(${PROJECT_NAME} saga_gdi)
endif()
