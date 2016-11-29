option(WITH_TOOL_IO_GRID_IMAGE "Check to install tool io_grid_image" OFF)

if (WITH_TOOL_IO_GRID_IMAGE OR WITH_ALL_TOOLS)
	project(io_grid_image)

	# load common tool template
	include(../../CMakePluginTemplate.cmake)
	# include directories of wxWidgets are already present
	find_package(wxWidgets REQUIRED)
	target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
endif()
