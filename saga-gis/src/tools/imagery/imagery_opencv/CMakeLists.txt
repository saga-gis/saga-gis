option(OpenCV_TOOLS "Build with OpenCV dependent tools" OFF)

if(WITH_TOOLS AND OpenCV_TOOLS)
	project(imagery_opencv)

	include(../../CMakePluginTemplate.cmake)	# load common tool template

	find_package(OpenCV REQUIRED)	# on Ubuntu package libopencv-dev
	target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
	if(MSVC)
		target_link_directories(${PROJECT_NAME} PRIVATE ${OpenCV_LIBRARIES})
		if(${CMAKE_CFG_INTDIR} MATCHES "Debug")
			target_link_libraries(${PROJECT_NAME} opencv_core430d opencv_imgproc430d opencv_ml430d opencv_calib3d430d)
		else()
			target_link_libraries(${PROJECT_NAME} opencv_core430 opencv_imgproc430 opencv_ml430 opencv_calib3d430)
		endif()
	else()
		target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
	endif()
endif()
