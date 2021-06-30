if(WITH_TOOLS AND WITH_GUI)
	project(pointcloud_viewer)
	include(../../CMakePluginTemplate.cmake)	# load common tool template
	find_package(wxWidgets REQUIRED QUIET)
	include(${wxWidgets_USE_FILE})
	target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
	target_link_libraries(${PROJECT_NAME} saga_gdi)
endif()
