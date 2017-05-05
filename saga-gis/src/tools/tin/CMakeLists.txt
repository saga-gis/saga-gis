message("folder: tools/tin")

add_subdirectory(tin_tools)
if(WITH_GDI)
	add_subdirectory(tin_viewer)
endif()
