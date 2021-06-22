option(WITH_TOOLS_RIEGL "Check to install tool riegl_io_rdb2_import" OFF)

if(WITH_TOOLS AND WITH_TOOLS_RIEGL)
	project(riegl_io_rdb2_import)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )

	# find rdb
	find_package(rdb REQUIRED)
	target_link_libraries(${PROJECT_NAME} rdbcpp)
endif()
