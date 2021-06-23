message(STATUS "folder: tools")

# Auto add all subdirectories
file(GLOB subdirectories *)

# Ubuntu 16.04 all tool dependencies
# sudo apt install unixodbc-dev libpq-dev libhpdf-dev libopencv-dev libvigraimpex-dev libfftw3-dev libgdal-dev liblas-dev libproj-dev
# optional
# sudo apt install libsvm-dev libqhull-dev libdxflib-dev

if(MSVC)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG          ${CMAKE_BINARY_DIR}/Debug/tools)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/RelWithDebInfo/tools)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     ${CMAKE_BINARY_DIR}/MinSizeRel/tools)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE        ${CMAKE_BINARY_DIR}/Release/tools)
endif()

# add all tool subdirectories with CMakeLists files
foreach(dir ${subdirectories})
	if(IS_DIRECTORY "${dir}" AND EXISTS "${dir}/CMakeLists.txt")
		message(STATUS "auto-add tool subdirectory: ${dir}")
		add_subdirectory ("${dir}")
	endif()
endforeach()
