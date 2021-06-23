# default tool (plugin) CMake script
message(STATUS "configure tool: ${PROJECT_NAME}")

# add all c, cpp, h and hpp files from the current directory
file(GLOB header_list *.hpp *.h)
file(GLOB source_list *.cpp *.c)

# add files in custom locations
if (SAGA_TOOL_CUSTOM_HEADER_LIST)
	list(APPEND header_list ${SAGA_TOOL_CUSTOM_HEADER_LIST})
endif()
if (SAGA_TOOL_CUSTOM_SOURCE_LIST)
	list(APPEND source_list ${SAGA_TOOL_CUSTOM_SOURCE_LIST})
endif()

# remove blacklisted files
foreach(elem ${SAGA_TOOL_BLACKLIST})
	list(REMOVE_ITEM source_list ${elem})
	list(REMOVE_ITEM header_list ${elem})
endforeach()

# create the tool library
add_library(${PROJECT_NAME} SHARED ${header_list} ${source_list})

# add saga_core to include directories
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
target_include_directories(${PROJECT_NAME} PRIVATE "${saga_core_include_dir}")

# set common compile flags
if(MSVC)
	target_compile_definitions(${PROJECT_NAME} PUBLIC -DUNICODE)
#	target_link_options(${PROJECT_NAME} PRIVATE /OUT:${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/tools/${PROJECT_NAME}.dll)

else() # if(NOT MSVC) # expect g++ on Linux
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS -fPIC)
	target_compile_definitions(${PROJECT_NAME} PUBLIC -D_SAGA_LINUX)
endif()

target_compile_definitions(${PROJECT_NAME} PUBLIC -D_TYPEDEF_BYTE -D_TYPEDEF_WORD)

# link saga_api
target_link_libraries(${PROJECT_NAME} saga_api)

install(TARGETS ${PROJECT_NAME} DESTINATION lib/saga)

# link_tester
if(WITH_LINK_TESTER)
	if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/MLB_Interface.h")
		message(WARNING "Can't build link_tester "
			"MLB_Interface missing: ${CMAKE_CURRENT_SOURCE_DIR}/MLB_Interface.h")
	else()
		add_executable(link_tester_${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/link_tester.cpp)
		target_link_libraries(link_tester_${PROJECT_NAME} ${PROJECT_NAME})
	endif()
endif()

# reset variables
set(SAGA_TOOL_CUSTOM_SOURCE_LIST)
set(SAGA_TOOL_CUSTOM_HEADER_LIST)
set(SAGA_TOOL_BLACKLIST)
