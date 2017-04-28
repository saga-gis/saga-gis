option(WITH_TOOL_RIEGL_IO_RDB2_IMPORT "Check to install tool riegl_io_rdb2_import" OFF)

if (WITH_TOOL_RIEGL_IO_RDB2_IMPORT OR WITH_ALL_TOOLS)
	project(riegl_io_rdb2_import)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )

	# find rdb
	find_package(rdb REQUIRED)
	target_link_libraries(${PROJECT_NAME} rdbcpp)
endif()
