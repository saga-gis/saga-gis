cmake_minimum_required(VERSION 2.8.12)

# Policy CMP0043 introduced in cmake version 3.0 IGNORES the use of COMPILE_DEFINITIONS
# suffixed variables, e.g. COMPILE_DEFINITIONS_DEBUG
# since cmake version >= 2.8.10 COMPILE_DEFINITIONS can be set by generator expressions.
if (POLICY CMP0043)
	cmake_policy(SET CMP0043 NEW)
endif (POLICY CMP0043)

# define project name and its description
project(saga-gis)
message(STATUS "folder: ${PROJECT_NAME}")
set(TOP_PROJECT ${PROJECT_NAME})
string(TOLOWER ${TOP_PROJECT} TOP_PROJECT_LOWER)
set(PROJECT_DESCRIPTION "saga-gis project")

# Set a default build type for single-configuration cmake generators if
# no build type is set.
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
	message(STATUS "Set default build type to Release")
	set(CMAKE_BUILD_TYPE Release)
endif()

# options
option (WITH_GUI "Build saga-gis with GUI" ON)
option (WITH_TOOLS "Build saga-gis with tools" ON)
option (WITH_ALL_TOOLS "Build all saga-gis tools" OFF)
option (WITH_LINK_TESTER "Check to enable testing if tools are successfully linking" OFF)
option (WITH_OPENMP "Compile with OpenMP optimizations" ON)

if(MSVC)
	option (PARALLEL_BUILD "Pass /MP to msbuild to compile in parallel" ON)
	if(PARALLEL_BUILD)
		add_definitions(/MP)
	endif()
endif()

# include version and debianize
include(version.cmake)

# include shared definitions and scripts
include(shared_cmake/scripts/require-cpp11.cmake)
include(shared_cmake/scripts/enable_warnings.cmake)
include(shared_cmake/scripts/windows_version_info.cmake)

# create windows resouce file
set(windows_version_rc "${CMAKE_BINARY_DIR}/win_version_info.rc")
gen_win_version_resource("${PROJECT_NAME}" "${windows_version_rc}"
	${SAGA_VERSION_MAJOR} ${SAGA_VERSION_MINOR} ${SAGA_VERSION_PATCH} ${GIT_COUNT}
	${SAGA_VERSION_FULL_STRING})

# include the generated header files
include_directories("${PROJECT_BINARY_DIR}")


# add local models to search path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

# find wxWidgets
find_package(wxWidgets REQUIRED)
include("${wxWidgets_USE_FILE}")

# compile with OpenMP optimizations
if (WITH_OPENMP)
	find_package(OpenMP QUIET REQUIRED)
	message(STATUS "compile with OpenMP optimizations")
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# add files to dummy target to show in IDE
file(GLOB CMAKE_FILES "cmake/*.cmake" "cmake/modules/*.cmake")
file(GLOB SHARED_CMAKE_FILES "shared_cmake/scripts/*.cmake")
add_custom_target(saga-other-files SOURCES
	.gitignore
	COPYING
	README
	src/gpl.txt
	src/lgpl.txt
	src/install.txt
	cmake/Config.cmake
	cmake/config.h.cmake
	${CMAKE_FILES}
	${SHARED_CMAKE_FILES}
)

# configure config.h
include(cmake/Config.cmake)
Config()

# add subdirectories
add_subdirectory(src)

# in case some automated tests will be written
#option(BUILD_TESTING "Build tests for saga-gis" ON)
#if(BUILD_TESTING)
#	 enable tests
#	enable_testing(true)
#	add_subdirectory(tests)
#endif()
