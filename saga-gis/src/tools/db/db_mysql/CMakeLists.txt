option(WITH_TOOL_DB_MYSQL "Check to install tool db_mysql" OFF)

if (WITH_TOOL_DB_MYSQL OR WITH_ALL_TOOLS)
	project(db_mysql)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )

	# load mysql
	# ubuntu 16.04 package libmysqlclient-dev
	find_package(MySQL REQUIRED)
	target_include_directories(${PROJECT_NAME} PRIVATE "${MySQL_INCLUDE_DIRS}")
	target_link_libraries(${PROJECT_NAME} "${MySQL_LIBRARIES}")
endif()
