if(WITH_TOOLS)
	find_package(Postgres)	# ubuntu package libpq-dev
	if(POSTGRES_FOUND)
		project(db_pgsql)
		include(../../CMakePluginTemplate.cmake)	# load common tool template
		target_include_directories(${PROJECT_NAME} PRIVATE "${POSTGRES_INCLUDE_DIR}")
		target_link_libraries(${PROJECT_NAME} "${POSTGRES_LIBRARY}")
	endif(POSTGRES_FOUND)
endif()
