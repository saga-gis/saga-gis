message("folder: tools/tin")

add_subdirectory(tin_tools)
if(WITH_GUI)
	add_subdirectory(tin_viewer)
endif()
