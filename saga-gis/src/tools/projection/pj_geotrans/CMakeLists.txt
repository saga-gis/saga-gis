option(WITH_SYSTEM_GEOTRANS "Check to use system version of geotrans instead of embedded copy" OFF)

if(WITH_TOOLS)
	project(pj_geotrans)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )

	# compile and link geotrans
	if(WITH_SYSTEM_GEOTRANS)
		find_package(geotrans REQUIRED)
		target_include_directories(${PROJECT_NAME} PRIVATE ${geotrans_INCLUDE_DIRS})
		target_link_libraries(${PROJECT_NAME} ${geotrans_LIBRARIES})
	else()
		add_subdirectory(geotrans)
		target_link_libraries(${PROJECT_NAME} geotrans)
	endif()
endif()
