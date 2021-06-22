if(WITH_TOOLS AND WITH_GUI)
	project(tin_viewer)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )

	# include directories of wxWidgets are already present
	find_package(wxWidgets REQUIRED)
	include(${wxWidgets_USE_FILE})
	target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})

	# link saga_gdi
	target_link_libraries(${PROJECT_NAME} saga_gdi)
endif()
