option(WITH_TOOLS_OPENCV "Check to create build targets for OpenCV tools" ON)

if(WITH_TOOLS_OPENCV)
	#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#
	if(MSVC AND DEPS_FROM_ENVARS)
		if("${CMAKE_GENERATOR_PLATFORM}" STREQUAL "Win32")
			set(LIB4SAGA $ENV{OPENCV_32})
		else()
			set(LIB4SAGA $ENV{OPENCV})
		endif()
		find_path(OpenCV_INCLUDE_DIRS NAMES opencv2/opencv.hpp PATHS "${LIB4SAGA}/include")
		find_path(OpenCV_LIBRARIES    NAMES opencv_core430.lib PATHS "${LIB4SAGA}/lib")
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

	find_package(OpenCV QUIET)	# on Ubuntu package libopencv-dev
	if(OpenCV_FOUND)
		project(imagery_opencv)
		include(../../CMakePluginTemplate.cmake)	# load common tool template
		target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
		target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
	endif()
endif()
