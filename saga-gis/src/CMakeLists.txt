message(STATUS "folder: src")

# add variable to store saga_core include directory
set(saga_core_include_dir "${CMAKE_CURRENT_SOURCE_DIR}/saga_core")

add_subdirectory(accessories)
add_subdirectory(saga_core)
add_subdirectory(tools)
add_subdirectory(dev_tools)
