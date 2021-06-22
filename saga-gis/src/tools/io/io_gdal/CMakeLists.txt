option(WITH_GDAL "Build with GDAL/OGR tools" ON)

if(WITH_GDAL)
	project(io_gdal)

	# load common tool template
	include(../../CMakePluginTemplate.cmake)

	# Ubuntu 16.04 package libgdal-dev
	find_package(gdal REQUIRED)
	target_include_directories(${PROJECT_NAME} PUBLIC ${gdal_INCLUDE_DIR})
	target_link_libraries(${PROJECT_NAME} ${gdal_LIBRARY})
#	message("[${PROJECT_NAME}] GDAL include: ${gdal_INCLUDE_DIR}")
#	message("[${PROJECT_NAME}] GDAL library: ${gdal_LIBRARY}")
endif()
