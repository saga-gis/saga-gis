option(LAS_TOOLS "Build with libLAS dependent tools" OFF)

if(WITH_TOOLS AND LAS_TOOLS)
	project(io_shapes_las)

	include(../../CMakePluginTemplate.cmake)	# load common tool template

	find_package(LAS REQUIRED)	# Ubuntu package liblas-dev
	target_include_directories(${PROJECT_NAME} PRIVATE "${LAS_INCLUDE_DIRS}")
	target_link_libraries(${PROJECT_NAME} ${LAS_LIBRARIES})
endif()
