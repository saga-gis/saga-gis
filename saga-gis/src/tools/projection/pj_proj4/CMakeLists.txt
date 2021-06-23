option(PROJ_TOOLS "Build with Proj tools" ON)

if(PROJ_TOOLS)
	project(pj_proj4)

	include(../../CMakePluginTemplate.cmake)	# load common tool template

	find_package(proj REQUIRED)	# Ubuntu package libproj-dev
	target_include_directories(${PROJECT_NAME} PRIVATE ${PROJ_INCLUDE_DIR})
	target_link_libraries(${PROJECT_NAME} ${PROJ_LIBRARY})

	add_definitions(/DPROJ6)
endif()
