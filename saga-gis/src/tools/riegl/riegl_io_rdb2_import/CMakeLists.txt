if(WITH_TOOLS)
	find_package(rdb QUIET)
	if(rdb_FOUND)
		project(riegl_io_rdb2_import)
		include(../../CMakePluginTemplate.cmake)	# load common tool template
		target_link_libraries(${PROJECT_NAME} rdbcpp)
	endif()
endif()
