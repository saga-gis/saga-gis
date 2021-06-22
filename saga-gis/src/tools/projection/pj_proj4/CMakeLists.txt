option(WITH_TOOLS_PROJ "Build with Proj tools" ON)

if(WITH_TOOLS_PROJ)
	project(pj_proj4)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )

	# link proj4
	# Ubuntu 16.04 package libproj-dev
	find_package(proj REQUIRED)
	target_include_directories(${PROJECT_NAME} PRIVATE ${proj_INCLUDE_DIRS})
	target_link_libraries(${PROJECT_NAME} ${proj_LIBRARIES})

	add_definitions(/DPROJ6)
endif()
