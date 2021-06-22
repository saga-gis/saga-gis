if(WITH_TOOLS)
	project(db_odbc)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )

	if(MSVC)
		target_link_libraries(${PROJECT_NAME} odbc32 odbccp32)
	else()
		# load odbc library
		# ubuntu 16.04 packages unixodbc-dev
		find_package(ODBC REQUIRED)
		#target_include_directories(${PROJECT_NAME} PRIVATE "${ODBC_INCLUDE_DIRS}")
		target_link_libraries(${PROJECT_NAME} "${ODBC_LIBRARIES}")
	endif()		
endif()
