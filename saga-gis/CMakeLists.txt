cmake_minimum_required(VERSION 3.14)

# Policy CMP0043 introduced in cmake version 3.0 IGNORES the use of COMPILE_DEFINITIONS
# suffixed variables, e.g. COMPILE_DEFINITIONS_DEBUG
# since cmake version >= 2.8.10 COMPILE_DEFINITIONS can be set by generator expressions.
if (POLICY CMP0043)
	cmake_policy(SET CMP0043 NEW)
endif (POLICY CMP0043)

# define project name and its description
project(saga)
message(STATUS "folder: ${PROJECT_NAME}")
set(TOP_PROJECT ${PROJECT_NAME})
string(TOLOWER ${TOP_PROJECT} TOP_PROJECT_LOWER)
set(PROJECT_DESCRIPTION "SAGA - System for Automated Geoscientific Analyses")

# Set a default build type for single-configuration cmake generators if
# no build type is set.
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
	message(STATUS "Set default build type to Release")
	set(CMAKE_BUILD_TYPE Release)
endif()


# Platform independent compiler requirements
set(CMAKE_CXX_STANDARD		 		14		CACHE STRING "C++ standard to be used")
set(CMAKE_CXX_STANDARD_REQUIRED		ON 		CACHE STRING "The C++ standard is required")

# Platform dependent compiler requirements
if(MSVC)
	set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

	option(DEPS_FROM_ENVARS "Library locations from environment" ON)

	option(PARALLEL_BUILD "Multi-processor compilation (/MP)" ON)
	if(PARALLEL_BUILD)
		add_definitions(/MP)
	endif()

	add_definitions(/utf-8)

	# default debug configuration to use: 'Program Database for Edit And Continue (/ZI)'
	set(CMAKE_C_FLAGS_DEBUG             "/MDd /ZI /Ob0 /Od /RTC1"       CACHE STRING "" FORCE)
	set(CMAKE_CXX_FLAGS_DEBUG           "/MDd /ZI /Ob0 /Od /RTC1"       CACHE STRING "" FORCE)
	set(CMAKE_EXE_LINKER_FLAGS_DEBUG    "/debug /INCREMENTAL /LTCG:OFF" CACHE STRING "" FORCE)
	set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL /LTCG:OFF" CACHE STRING "" FORCE)
	set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL /LTCG:OFF" CACHE STRING "" FORCE)
else()
	# Unix Flags
	set(CMAKE_CXX_FLAGS_RELEASE         "-O3 -Wall -DNDEBUG"            CACHE STRING "" FORCE)
	set(CMAKE_CXX_FLAGS_DEBUG           "-Og -Wall -D_DEBUG -ggdb"      CACHE STRING "" FORCE)
endif()

# options
option(WITH_GUI "Build SAGA with GUI" ON)
option(WITH_TOOLS "Build SAGA with tools" ON)
option(OpenMP_SUPPORT "Compile with OpenMP optimizations" ON)
#option(WITH_LINK_TESTER "Check to enable testing if tools are successfully linking" OFF)

# include version and debianize
include(version.cmake)

# include shared definitions and scripts
include(cmake/shared/enable_warnings.cmake)
include(cmake/shared/windows_version_info.cmake)

# Use GNU install dirs to support multiarch
include(cmake/modules/GNUInstallDirs.cmake)

# clang uses this compile commands for the language server protocol
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 

# create windows resource file
set(windows_version_rc "${CMAKE_BINARY_DIR}/win_version_info.rc")
gen_win_version_resource("${PROJECT_NAME}" "${windows_version_rc}"
	${SAGA_VERSION_MAJOR} ${SAGA_VERSION_MINOR} ${SAGA_VERSION_PATCH} ${GIT_COUNT}
	${SAGA_VERSION_FULL_STRING})

# include the generated header files
include_directories("${PROJECT_BINARY_DIR}")


# add local models to search path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

# find wxWidgets
#find_package(wxWidgets REQUIRED)
#include("${wxWidgets_USE_FILE}")

# compile with OpenMP optimizations
if(OpenMP_SUPPORT)
	find_package(OpenMP QUIET REQUIRED)
	if(OPENMP_FOUND)
		message(STATUS "compile with OpenMP optimizations")
		set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   ${OpenMP_C_FLAGS}")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
		if(APPLE)
			set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   ${OpenMP_C_FLAGS}   -I${OpenMP_C_INCLUDE_DIR}")
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -I${OpenMP_CXX_INCLUDE_DIR}")
			cmake_path(GET OpenMP_libomp_LIBRARY PARENT_PATH OpenMP_LIBRARY_DIR)
			set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS}    -L${OpenMP_LIBRARY_DIR} -lomp")
			set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -L${OpenMP_LIBRARY_DIR} -lomp")
		endif(APPLE)
	else()
		message(STATUS "OpenMP not found!")
	endif(OPENMP_FOUND)
endif(OpenMP_SUPPORT)

# add files to dummy target to show in IDE
file(GLOB CMAKE_FILES "cmake/*.cmake" "cmake/modules/*.cmake")
file(GLOB SHARED_CMAKE_FILES "cmake/shared/*.cmake")
add_custom_target(saga-other-files SOURCES
	.gitignore
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
