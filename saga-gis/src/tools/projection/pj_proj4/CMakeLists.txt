option(WITH_TOOLS_PROJ "Check to create build targets for Proj tools" ON)

if(WITH_TOOLS_PROJ)
	find_package(Proj QUIET)	# Ubuntu package libproj-dev
	if(PROJ_FOUND)
		project(pj_proj4)
		include(../../CMakePluginTemplate.cmake)	# load common tool template
		target_include_directories(${PROJECT_NAME} PRIVATE ${PROJ_INCLUDE_DIR})
		target_link_libraries(${PROJECT_NAME} ${PROJ_LIBRARY})
	endif()
endif()
