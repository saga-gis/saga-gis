cmake_minimum_required(VERSION 2.8.12)

#________________________________________
# define the tool library project
project(my_tool_library)

set(TOP_PROJECT ${PROJECT_NAME})
set(PROJECT_DESCRIPTION "My Tool Library")

#________________________________________
# Set a default build type for single-configuration cmake generators if no build type is set.
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
	message(STATUS "Set default build type to Release")
	set(CMAKE_BUILD_TYPE Release)
endif()

#________________________________________
if(MSVC)
	set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

	option(PARALLEL_BUILD "Multi-processor compilation (/MP)" ON)
	if(PARALLEL_BUILD)
		add_definitions(/MP)
	endif()

	# default debug configuration to use: 'Program Database for Edit And Continue (/ZI)'
	set(CMAKE_C_FLAGS_DEBUG             "/MDd /ZI /Ob0 /Od /RTC1"       CACHE STRING "" FORCE)
	set(CMAKE_CXX_FLAGS_DEBUG           "/MDd /ZI /Ob0 /Od /RTC1"       CACHE STRING "" FORCE)
	set(CMAKE_EXE_LINKER_FLAGS_DEBUG    "/debug /INCREMENTAL /LTCG:OFF" CACHE STRING "" FORCE)
	set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL /LTCG:OFF" CACHE STRING "" FORCE)
	set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL /LTCG:OFF" CACHE STRING "" FORCE)
endif()

#________________________________________
# compile with OpenMP optimizations
option(OpenMP_SUPPORT "Compile with OpenMP optimizations" ON)

if(OpenMP_SUPPORT)
	message(STATUS "compile with OpenMP optimizations")
	find_package(OpenMP QUIET REQUIRED)
	set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   ${OpenMP_C_FLAGS}"  )
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

#________________________________________
# add all source files from the current
# directory and create the tool library
file(GLOB header_list *.hpp *.h)
file(GLOB source_list *.cpp *.c)

add_library(${PROJECT_NAME} SHARED ${header_list} ${source_list})

#________________________________________
# add saga_core to include directories
find_path   (SAGA_API_INCLUDE NAMES saga_api/saga_api.h      PATHS "$ENV{SAGA}/src/saga_core")
find_library(SAGA_API_LIBRARY NAMES saga_api.lib libsaga_api PATHS "$ENV{SAGA}/bin/saga_x64")
mark_as_advanced(SAGA_API_INCLUDE SAGA_API_LIBRARY)

#_CRT_SECURE_NO_WARNINGS
target_compile_definitions(${PROJECT_NAME} PUBLIC -D_TYPEDEF_BYTE -D_TYPEDEF_WORD)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(${PROJECT_NAME} PRIVATE "${SAGA_API_INCLUDE}")

target_link_libraries     (${PROJECT_NAME}         "${SAGA_API_LIBRARY}")

# set common compile flags
if(MSVC)
	target_compile_definitions(${PROJECT_NAME} PUBLIC -D_SAGA_MSW -DUNICODE)
else() # if(NOT MSVC) # expect g++ on Linux
#	target_compile_definitions(${PROJECT_NAME} PUBLIC -D_SAGA_LINUX)
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS -fPIC)
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION lib/saga)
