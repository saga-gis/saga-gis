message(STATUS "folder: tools")

# Tool dependencies (Ubuntu, sudo apt install...)
#   libgdal-dev
#   libproj-dev
#   unixodbc-dev
#   libpq-dev
#   libhpdf-dev
#   libopencv-dev
#   libvigraimpex-dev libfftw3-dev
#   liblas-dev
#   libpdal-dev
#
# System alternatives to build-in code
#   libcurl-dev
#   libsvm-dev
#   libqhull-dev
#   libdxflib-dev

if(MSVC)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG          ${CMAKE_BINARY_DIR}/Debug/tools)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/RelWithDebInfo/tools)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     ${CMAKE_BINARY_DIR}/MinSizeRel/tools)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE        ${CMAKE_BINARY_DIR}/Release/tools)
endif()

# Add all tool subdirectories with CMakeLists.txt files
file(GLOB subdirectories *)

foreach(dir ${subdirectories})
	if(IS_DIRECTORY "${dir}" AND EXISTS "${dir}/CMakeLists.txt")
		add_subdirectory("${dir}")
	endif()
endforeach()
