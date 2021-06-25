if(WITH_TOOLS)
	find_package(GDAL)	# Ubuntu package libgdal-dev
	if(GDAL_FOUND)
		project(io_gdal)
		include(../../CMakePluginTemplate.cmake)	# load common tool template
		target_include_directories(${PROJECT_NAME} PUBLIC ${GDAL_INCLUDE_DIR})
		target_link_libraries(${PROJECT_NAME} ${GDAL_LIBRARY})
	#	message("[${PROJECT_NAME}] GDAL include: ${GDAL_INCLUDE_DIR}")
	#	message("[${PROJECT_NAME}] GDAL library: ${GDAL_LIBRARY}")
	endif()
endif()
