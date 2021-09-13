option(WITH_DEV_TOOLS "Check to create build targets for SAGA development tools" OFF)

if(WITH_DEV_TOOLS)
	project(dev_tools)

	include(../../CMakePluginTemplate.cmake) # load common tool template

	find_package(wxWidgets REQUIRED QUIET)
	include(${wxWidgets_USE_FILE})
	target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
endif()
