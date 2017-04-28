option(WITH_TOOL_IO_GDAL "Check to install tool io_gdal" OFF)

if (WITH_TOOL_IO_GDAL OR WITH_ALL_TOOLS)
	project(io_gdal)

	# load common tool template
	include(../../CMakePluginTemplate.cmake)
	# link GDAL library
	# Ubuntu 16.04 package libgdal-dev
	find_package(GDAL REQUIRED)
	target_include_directories(${PROJECT_NAME} PUBLIC ${GDAL_INCLUDE_DIRS})
	target_link_libraries(${PROJECT_NAME} ${GDAL_LIBRARIES})
endif()
