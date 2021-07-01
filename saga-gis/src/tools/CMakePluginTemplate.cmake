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
#	target_compile_definitions(${PROJECT_NAME} PUBLIC -D_SAGA_MSW -DUNICODE)
else() # if(NOT MSVC) # expect g++ on Linux
#	target_compile_definitions(${PROJECT_NAME} PUBLIC -D_SAGA_LINUX)
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS -fPIC)
endif()

target_compile_definitions(${PROJECT_NAME} PUBLIC -D_TYPEDEF_BYTE -D_TYPEDEF_WORD)

# link saga_api
target_link_libraries(${PROJECT_NAME} saga_api)

install(TARGETS ${PROJECT_NAME} DESTINATION lib/saga)

# reset variables
set(SAGA_TOOL_CUSTOM_SOURCE_LIST)
set(SAGA_TOOL_CUSTOM_HEADER_LIST)
set(SAGA_TOOL_BLACKLIST)
