option(WITH_TOOL_IO_PDAL "Check to install tool io_pdal" OFF)

if (WITH_TOOL_IO_PDAL OR WITH_ALL_TOOLS)
	project(io_pdal)

	# load common tool template
	include(../../CMakePluginTemplate.cmake)
	# link PDAL library
	find_package(PDAL REQUIRED)
	target_include_directories(${PROJECT_NAME} PUBLIC ${PDAL_INCLUDE_DIRS})
	target_link_libraries(${PROJECT_NAME} ${PDAL_LIBRARIES})
endif()
