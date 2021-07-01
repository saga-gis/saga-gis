option(WITH_DEV_TOOLS "Check to create build targets for SAGA development tools" OFF)

if(WITH_DEV_TOOLS)
	if(MSVC)
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG          ${CMAKE_BINARY_DIR}/saga_${CMAKE_GENERATOR_PLATFORM}_d/tools)
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/saga_${CMAKE_GENERATOR_PLATFORM}_rd/tools)
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     ${CMAKE_BINARY_DIR}/saga_${CMAKE_GENERATOR_PLATFORM}_minsize/tools)
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE        ${CMAKE_BINARY_DIR}/saga_${CMAKE_GENERATOR_PLATFORM}/tools)
	endif()

	project(dev_tools)

	include(../tools/CMakePluginTemplate.cmake)	# load common tool template

	find_package(wxWidgets REQUIRED QUIET)
	include(${wxWidgets_USE_FILE})
	target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
endif()
