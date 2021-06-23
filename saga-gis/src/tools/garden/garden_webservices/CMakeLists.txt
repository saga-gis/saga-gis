if(WITH_TOOLS)
	project(garden_webservices)

	include(../../CMakePluginTemplate.cmake)	# load common tool template

	find_package(wxWidgets REQUIRED)
	include("${wxWidgets_USE_FILE}")
	target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
endif()
