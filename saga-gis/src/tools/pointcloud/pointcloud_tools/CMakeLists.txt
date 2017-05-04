option(WITH_TOOL_POINTCLOUD_TOOLS "Check to install tool pointcloud_tools" OFF)

if (WITH_TOOL_POINTCLOUD_TOOLS OR WITH_ALL_STANDARD_TOOLS OR WITH_ALL_TOOLS)
	project(pointcloud_tools)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )

	# include directories of wxWidgets are already present
	find_package(wxWidgets REQUIRED)
	target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
endif()
