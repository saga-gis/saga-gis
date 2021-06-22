option(WITH_TOOLS_PDAL "Build with PDAL dependent tools" OFF)

if(WITH_TOOLS AND WITH_TOOLS_PDAL)
	project(io_pdal)

	# load common tool template
	include(../../CMakePluginTemplate.cmake)

	# link PDAL library
	find_package(PDAL REQUIRED)
	target_include_directories(${PROJECT_NAME} PUBLIC ${PDAL_INCLUDE_DIRS})
	target_link_libraries(${PROJECT_NAME} ${PDAL_LIBRARIES})
endif()
