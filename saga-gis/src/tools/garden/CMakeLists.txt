message("folder: tools/garden")

if(WITH_GUI)
	add_subdirectory(garden_3d_viewer)
	add_subdirectory(garden_webservices)
endif()
add_subdirectory(garden_fractals)
add_subdirectory(garden_games)
add_subdirectory(garden_learn_to_program)
