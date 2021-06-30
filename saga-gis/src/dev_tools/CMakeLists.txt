option(WITH_DEV_TOOLS "Check to create build targets for SAGA development tools" OFF)

if(WITH_DEV_TOOLS)
	if(MSVC)
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG          ${CMAKE_BINARY_DIR}/Debug/tools)
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/RelWithDebInfo/tools)
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     ${CMAKE_BINARY_DIR}/MinSizeRel/tools)
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE        ${CMAKE_BINARY_DIR}/Release/tools)
	endif()

	project(dev_tools)

	# add all c, cpp, h and hpp files from the current directory
	file(GLOB header_list *.hpp *.h)
	file(GLOB source_list *.cpp *.c)

	# create the tool library
	add_library(${PROJECT_NAME} SHARED ${header_list} ${source_list})

	# add saga_core to include directories
	target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
	target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
	target_include_directories(${PROJECT_NAME} PRIVATE "${saga_core_include_dir}")

	# set common compile flags
	if(MSVC)
		target_compile_definitions(${PROJECT_NAME} PUBLIC -D_SAGA_MSW -DUNICODE)

	else() # if(NOT MSVC) # expect g++ on Linux
		set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS -fPIC)
		target_compile_definitions(${PROJECT_NAME} PUBLIC -D_SAGA_LINUX)
	endif()

	target_compile_definitions(${PROJECT_NAME} PUBLIC -D_TYPEDEF_BYTE -D_TYPEDEF_WORD)

	find_package(wxWidgets REQUIRED QUIET)
	include(${wxWidgets_USE_FILE})
	target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})

	# link saga_api
	target_link_libraries(${PROJECT_NAME} saga_api)

	install(TARGETS ${PROJECT_NAME} DESTINATION lib/saga)
endif()
