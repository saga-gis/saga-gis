if(WITH_TOOLS)
	find_package(VIGRA QUIET)	# on Ubuntu package libvigraimpex-dev
	if(VIGRA_FOUND)
		project(imagery_vigra)
		include(../../CMakePluginTemplate.cmake)	# load common tool template
		target_include_directories(${PROJECT_NAME} PRIVATE ${VIGRA_INCLUDES})
		target_link_libraries(${PROJECT_NAME} ${VIGRA_LIBRARIES})
	endif()
endif()
