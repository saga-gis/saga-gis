if(WITH_TOOLS)
	project(statistics_kriging)
		
	include(../../CMakePluginTemplate.cmake)	# load common tool template
	find_package(wxWidgets REQUIRED QUIET)
	include(${wxWidgets_USE_FILE})
	target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
	
	if(WITH_GUI)
		target_compile_definitions(${PROJECT_NAME} PUBLIC -DWITH_GUI)
		target_link_libraries(${PROJECT_NAME} saga_gdi)
	endif()
endif()
