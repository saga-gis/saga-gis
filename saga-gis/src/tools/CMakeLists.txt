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
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG          ${CMAKE_BINARY_DIR}/saga_${CMAKE_GENERATOR_PLATFORM}_d/tools)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/saga_${CMAKE_GENERATOR_PLATFORM}_rd/tools)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     ${CMAKE_BINARY_DIR}/saga_${CMAKE_GENERATOR_PLATFORM}_minsize/tools)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE        ${CMAKE_BINARY_DIR}/saga_${CMAKE_GENERATOR_PLATFORM}/tools)
endif()

# Add all tool subdirectories with CMakeLists.txt files
file(GLOB subdirectories *)

foreach(dir ${subdirectories})
	if(IS_DIRECTORY "${dir}" AND EXISTS "${dir}/CMakeLists.txt")
		add_subdirectory("${dir}")
	endif()
endforeach()
