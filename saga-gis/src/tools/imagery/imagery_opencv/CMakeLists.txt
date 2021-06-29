if(WITH_TOOLS)
	#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#
	if(MSVC)
		find_path(OpenCV_INCLUDE_DIRS NAMES opencv2/opencv.hpp PATHS "$ENV{OPENCV}/include")
		find_path(OpenCV_LIBRARIES    NAMES opencv_core430.lib PATHS "$ENV{OPENCV}/lib")
		include(FindPackageHandleStandardArgs)
		FIND_PACKAGE_HANDLE_STANDARD_ARGS(OpenCV REQUIRED_VARS OpenCV_LIBRARIES OpenCV_INCLUDE_DIRS)
		mark_as_advanced(OpenCV_INCLUDE_DIRS OpenCV_LIBRARIES)
		if(OpenCV_FOUND)
			project(imagery_opencv)
			include(../../CMakePluginTemplate.cmake)	# load common tool template
			target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
			target_link_directories(${PROJECT_NAME} PRIVATE ${OpenCV_LIBRARIES})
			target_link_libraries(${PROJECT_NAME}
				optimized opencv_core430    debug opencv_core430d
				optimized opencv_imgproc430 debug opencv_imgproc430d
				optimized opencv_ml430      debug opencv_ml430d
				optimized opencv_calib3d430 debug opencv_calib3d430d
			)
			return()
		endif()
	endif()
	#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#

	find_package(OpenCV)	# on Ubuntu package libopencv-dev
	if(OpenCV_FOUND)
		project(imagery_opencv)
		include(../../CMakePluginTemplate.cmake)	# load common tool template
		target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
		target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
	endif()
endif()
