if(WITH_TOOLS)
	find_package(rdb QUIET)
	if(rdb_FOUND)
		project(io_riegl_rdb)
		include_directories(rapidjson/include)
		include(../../CMakePluginTemplate.cmake)	# load common tool template
		target_link_libraries(${PROJECT_NAME} rdb)
	endif()
	
	if(NOT MSVC)
		find_package(rdb QUIET)
		if(rdb_FOUND)
			project(io_riegl_rdb)
			include_directories(rapidjson/include)
			include(../../CMakePluginTemplate.cmake)	# load common tool template
			target_link_libraries(${PROJECT_NAME} rdbcpp)
		endif()
	endif()
endif()
