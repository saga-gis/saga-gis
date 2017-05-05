option(WITH_TOOL_GARDEN_WEBSERVICES "Check to install tool garden_webservices" OFF)

if (WITH_TOOL_GARDEN_WEBSERVICES OR WITH_ALL_STANDARD_TOOLS OR WITH_ALL_TOOLS)
	project(garden_webservices)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )

	# include directories of wxWidgets are already present but explicitely include them
	find_package(wxWidgets REQUIRED)
	target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
endif()
