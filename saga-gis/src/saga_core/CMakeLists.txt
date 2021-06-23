message(STATUS "folder: saga_core")

if(NOT MSVC) # unix like systems
	# define lib and share path for saga_cmd and saga_gui
	set(MTOOLSPATH "TOOLS_PATH=\"${CMAKE_INSTALL_PREFIX}/lib/saga\"")
	set(MSHAREPATH "SHARE_PATH=\"${CMAKE_INSTALL_PREFIX}/share/saga\"")
endif()

if(MSVC)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

find_package(wxWidgets REQUIRED)
include("${wxWidgets_USE_FILE}")

# include subdirectories
add_subdirectory(saga_api)
add_subdirectory(saga_cmd)

if(WITH_GUI)
	add_subdirectory(saga_gdi)
	add_subdirectory(saga_gui)
endif()
