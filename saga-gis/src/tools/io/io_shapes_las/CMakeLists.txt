option(WITH_TOOLS_LIBLAS "Build with libLAS dependent tools" OFF)

if(WITH_TOOLS AND WITH_TOOLS_LIBLAS)
	project(io_shapes_las)

	# load common tool template
	include(../../CMakePluginTemplate.cmake)

	# link las library
	# Ubuntu 16.04 package liblas-dev
	find_package(LAS REQUIRED)
	target_include_directories(${PROJECT_NAME} PRIVATE "${LAS_INCLUDE_DIRS}")
	target_link_libraries(${PROJECT_NAME} ${LAS_LIBRARIES})
endif()
