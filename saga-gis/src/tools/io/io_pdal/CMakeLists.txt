if(WITH_TOOLS)
	find_package(PDAL)	# link PDAL library
	if(PDAL_FOUND)
		project(io_pdal)
		include(../../CMakePluginTemplate.cmake)	# load common tool template
		target_include_directories(${PROJECT_NAME} PUBLIC ${PDAL_INCLUDE_DIR})
		target_link_libraries(${PROJECT_NAME} ${PDAL_LIBRARIES})
	endif()
endif()
