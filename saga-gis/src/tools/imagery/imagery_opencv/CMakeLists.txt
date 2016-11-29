option(WITH_TOOL_IMAGERY_OPENCV "Check to install tool imagery_opencv" OFF)

if (WITH_TOOL_IMAGERY_OPENCV OR WITH_ALL_TOOLS)
	project(imagery_opencv)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )

	# link OpenCV library
	# on Ubuntu 16.04 package libopencv-dev
	find_package(OpenCV REQUIRED)
	target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
	target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
endif()
