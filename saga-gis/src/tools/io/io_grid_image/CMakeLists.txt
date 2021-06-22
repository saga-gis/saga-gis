if(WITH_TOOLS)
	project(io_grid_image)

	# load common tool template
	include(../../CMakePluginTemplate.cmake)

	# include directories of wxWidgets are already present
	find_package(wxWidgets REQUIRED)
	include("${wxWidgets_USE_FILE}")
	target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
endif()
