message("folder: tools/pointcloud")

add_subdirectory(pointcloud_tools)
if(WITH_GUI)
	add_subdirectory(pointcloud_viewer)
endif()
