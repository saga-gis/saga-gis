message(STATUS "folder: tools")

# Auto add all subdirectories
file(GLOB subdirectories *)

# Ubuntu 16.04 all tool dependencies
# sudo apt install unixodbc-dev  libpq-dev and postgresql-server-dev-9.5  libhpdf-dev  libopencv-dev  libvigraimpex-dev libfftw3-dev  libgdal-dev  liblas-dev  libproj-dev
# optional
# sudo apt install  libsvm-dev  libqhull-dev  libdxflib-dev

# add all tool subdirectories with CMakeLists files
if(MSVC)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

foreach(dir ${subdirectories})
	if(IS_DIRECTORY "${dir}" AND EXISTS "${dir}/CMakeLists.txt")
		message(STATUS "auto-add tool subdirectory: ${dir}")
		add_subdirectory ("${dir}")
	endif()
endforeach()
