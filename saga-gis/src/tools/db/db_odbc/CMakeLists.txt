if(WITH_TOOLS)
	if(MSVC)
		project(db_odbc)
		include(../../CMakePluginTemplate.cmake)	# load common tool template
		target_link_libraries(${PROJECT_NAME} odbc32 odbccp32)
	else()
		find_package(ODBC)	# ubuntu package unixodbc-dev
		if(ODBC_FOUND)
			project(db_odbc)
			include(../../CMakePluginTemplate.cmake)	# load common tool template
			target_include_directories(${PROJECT_NAME} PRIVATE "${ODBC_INCLUDE_DIRS}")
			target_link_libraries(${PROJECT_NAME} "${ODBC_LIBRARIES}")
		endif()
	endif()		
endif()
