if(WITH_TOOLS)
	find_package(OpenCV)	# on Ubuntu package libopencv-dev
	if(OpenCV_FOUND)
		project(imagery_opencv)
		include(../../CMakePluginTemplate.cmake)	# load common tool template
		target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
		if(MSVC)
			target_link_directories(${PROJECT_NAME} PRIVATE ${OpenCV_LIBRARIES})
			target_link_libraries(${PROJECT_NAME}
				optimized opencv_core430    debug opencv_core430d
				optimized opencv_imgproc430 debug opencv_imgproc430d
				optimized opencv_ml430      debug opencv_ml430d
				optimized opencv_calib3d430 debug opencv_calib3d430d
			)
		else()
			target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
		endif()
	endif()
endif()
