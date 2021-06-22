option(WITH_TOOLS_OPENCV "Build with OpenCV dependent tools" OFF)

if(WITH_TOOLS AND WITH_TOOLS_OPENCV)
	project(imagery_opencv)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )

	# link OpenCV library
	# on Ubuntu 16.04 package libopencv-dev
	find_package(OpenCV REQUIRED)
	target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
	target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
endif()
