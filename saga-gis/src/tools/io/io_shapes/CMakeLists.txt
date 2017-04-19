option(WITH_TOOL_IO_SHAPES "Check to install tool io_shapes" OFF)

if (WITH_TOOL_IO_SHAPES OR WITH_ALL_TOOLS)
	project(io_shapes)

	# load common tool template
	include(../../CMakePluginTemplate.cmake)
	# include directories of wxWidgets are already present
	find_package(wxWidgets REQUIRED)
	target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
endif()
