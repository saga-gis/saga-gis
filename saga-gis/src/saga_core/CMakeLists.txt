message(STATUS "folder: saga_core")

# define lib and share path for saga_cmd and saga_gui
set(MLIBPATH "MODULE_LIBRARY_PATH=\"${CMAKE_INSTALL_PREFIX}/lib/saga\"")
set(MSHAREPATH "SHARE_PATH=\"${CMAKE_INSTALL_PREFIX}/share/saga\"")

# include subdirectories
add_subdirectory(saga_api)
add_subdirectory(saga_cmd)

if(WITH_GDI)
	add_subdirectory(saga_gdi)
	if(WITH_GUI)
		add_subdirectory(saga_gui)
	endif()
endif()

