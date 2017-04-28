option(WITH_TOOL_DB_PGSQL "Check to install tool db_pgsql" OFF)

if (WITH_TOOL_DB_PGSQL OR WITH_ALL_TOOLS)
	project(db_pgsql)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )

	# load postgresql
	# ubuntu 16.04 packages libpq-dev and postgresql-server-dev-9.5
	find_package(PostgreSQL REQUIRED)
	target_include_directories(${PROJECT_NAME} PRIVATE "${PostgreSQL_INCLUDE_DIRS}")
	target_link_libraries(${PROJECT_NAME} "${PostgreSQL_LIBRARIES}")
endif()
