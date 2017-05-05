option(WITH_TOOL_IO_VIRTUAL "Check to install tool io_virtual" OFF)

if (WITH_TOOL_IO_VIRTUAL OR WITH_ALL_STANDARD_TOOLS OR WITH_ALL_TOOLS)
	project(io_virtual)

	# load common tool template
	include(../../CMakePluginTemplate.cmake)
	# include directories of wxWidgets are already present
	find_package(wxWidgets REQUIRED)
	target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
endif()
