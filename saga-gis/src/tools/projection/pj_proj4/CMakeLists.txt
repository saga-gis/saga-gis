option(WITH_TOOL_PJ_PROJ4 "Check to install tool pj_proj4" OFF)

if (WITH_TOOL_PJ_PROJ4 OR WITH_ALL_TOOLS)
	project(pj_proj4)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )

	# link proj4
	# Ubuntu 16.04 package libproj-dev
	find_package(proj REQUIRED)
	target_include_directories(${PROJECT_NAME} PRIVATE ${proj_INCLUDE_DIRS})
	target_link_libraries(${PROJECT_NAME} ${proj_LIBRARIES})
endif()
