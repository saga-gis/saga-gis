option(WITH_TOOL_IO_WEBSERVICES "Check to install tool io_webservices" OFF)

if (WITH_TOOL_IO_WEBSERVICES OR WITH_ALL_STANDARD_TOOLS OR WITH_ALL_TOOLS)
	project(io_webservices)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )

	# include directories of wxWidgets are already present but explicitely include them
	find_package(wxWidgets REQUIRED)
	target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
endif()
