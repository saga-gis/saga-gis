option(PDAL_TOOLS "Build with PDAL dependent tools" OFF)

if(WITH_TOOLS AND PDAL_TOOLS)
	project(io_pdal)

	include(../../CMakePluginTemplate.cmake)	# load common tool template

	find_package(PDAL REQUIRED)	# link PDAL library
	target_include_directories(${PROJECT_NAME} PUBLIC ${PDAL_INCLUDE_DIR})
	target_link_libraries(${PROJECT_NAME} ${PDAL_LIB_CPP} ${PDAL_LIB_UTIL})
endif()
