message(STATUS "folder: saga_core")

# define lib and share path for saga_cmd and saga_gui
set(MLIBPATH "MODULE_LIBRARY_PATH=\"${CMAKE_INSTALL_PREFIX}/lib/saga\"")
set(MSHAREPATH "SHARE_PATH=\"${CMAKE_INSTALL_PREFIX}/share/saga\"")

find_package(wxWidgets REQUIRED)
include("${wxWidgets_USE_FILE}")

if(MSVC)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

# include subdirectories
add_subdirectory(saga_api)
add_subdirectory(saga_cmd)

if(WITH_GUI)
	add_subdirectory(saga_gdi)
	add_subdirectory(saga_gui)
endif()
