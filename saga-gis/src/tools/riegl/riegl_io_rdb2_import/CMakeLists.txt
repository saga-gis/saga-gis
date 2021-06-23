option(RIEGL_TOOLS "Check to install tool riegl_io_rdb2_import" OFF)

if(WITH_TOOLS AND RIEGL_TOOLS)
	project(riegl_io_rdb2_import)

	include(../../CMakePluginTemplate.cmake)	# load common tool template

	find_package(rdb REQUIRED)
	target_link_libraries(${PROJECT_NAME} rdbcpp)
endif()
