option(PostgreSQL_TOOLS "Build with Postgres tools" ON)

if(WITH_TOOLS AND PostgreSQL_TOOLS)
	project(db_pgsql)

	include(../../CMakePluginTemplate.cmake)	# load common tool template

	find_package(PostgreSQL REQUIRED)	# ubuntu package libpq-dev
	target_include_directories(${PROJECT_NAME} PRIVATE "${PostgreSQL_INCLUDE_DIRS}")
	target_link_libraries(${PROJECT_NAME} "${PostgreSQL_LIBRARIES}")
endif()
