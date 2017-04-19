option(WITH_TOOL_TIN_VIEWER "Check to install tool tin_viewer" OFF)

if (WITH_TOOL_TIN_VIEWER OR WITH_ALL_TOOLS)
	project(tin_viewer)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )

	# include directories of wxWidgets are already present
	find_package(wxWidgets REQUIRED)
	target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})

	# link saga_gdi
	target_link_libraries(${PROJECT_NAME} saga_gdi)
endif()
