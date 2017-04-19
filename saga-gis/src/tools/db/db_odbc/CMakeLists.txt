option(WITH_TOOL_DB_ODBC "Check to install tool db_odbc" OFF)

if (WITH_TOOL_DB_ODBC OR WITH_ALL_TOOLS)
	project(db_odbc)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )

	# load odbc library
	# ubuntu 16.04 packages unixodbc-dev
	find_package(ODBC REQUIRED)
	#target_include_directories(${PROJECT_NAME} PRIVATE "${ODBC_INCLUDE_DIRS}")
	target_link_libraries(${PROJECT_NAME} "${ODBC_LIBRARIES}")
endif()
