option(GDAL_TOOLS "Build with GDAL/OGR tools" ON)

if(WITH_TOOLS AND GDAL_TOOLS)
	project(io_gdal)

	include(../../CMakePluginTemplate.cmake)	# load common tool template

	find_package(gdal REQUIRED)	# Ubuntu package libgdal-dev
	target_include_directories(${PROJECT_NAME} PUBLIC ${GDAL_INCLUDE_DIR})
	target_link_libraries(${PROJECT_NAME} ${GDAL_LIBRARY})
#	message("[${PROJECT_NAME}] GDAL include: ${GDAL_INCLUDE_DIR}")
#	message("[${PROJECT_NAME}] GDAL library: ${GDAL_LIBRARY}")
endif()
