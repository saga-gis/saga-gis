if(WITH_TOOLS)
	project(io_webservices)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )

	# Ubuntu 16.04 package libcurl-dev
	find_package(curl)
	if(curl_FOUND)
		target_include_directories(${PROJECT_NAME} PRIVATE ${curl_INCLUDE_DIR})
		target_link_libraries(${PROJECT_NAME} ${curl_LIBRARY})
		add_definitions(/DHAVE_LIBCURL)
	#	message("[${PROJECT_NAME}] with curl, include: ${curl_INCLUDE_DIR}")
	#	message("[${PROJECT_NAME}] with curl, library: ${curl_LIBRARY}")
	endif()
endif()
