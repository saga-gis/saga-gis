if(WITH_TOOLS)
	find_package(LAS)	# Ubuntu package liblas-dev
	if(LAS_FOUND)
		project(io_shapes_las)
		include(../../CMakePluginTemplate.cmake)	# load common tool template
		target_include_directories(${PROJECT_NAME} PRIVATE "${LAS_INCLUDE_DIRS}")
		target_link_libraries(${PROJECT_NAME} ${LAS_LIBRARIES})
	endif()
endif()
