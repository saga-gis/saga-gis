if(WITH_TOOLS)
	project(io_webservices)
	include(../../CMakePluginTemplate.cmake)	# load common tool template
	find_package(CURL)	# Ubuntu package libcurl-dev
	if(CURL_FOUND)
		target_include_directories(${PROJECT_NAME} PRIVATE ${CURL_INCLUDE_DIRS})
		target_link_libraries(${PROJECT_NAME} ${CURL_LIBRARIES})
		add_definitions(/DHAVE_LIBCURL)
	#	message("[${PROJECT_NAME}] with curl, include: ${CURL_INCLUDE_DIR}")
	#	message("[${PROJECT_NAME}] with curl, library: ${CURL_LIBRARY}")
	endif()
endif()
