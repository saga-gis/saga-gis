project(accessories)
message(STATUS "folder: ${PROJECT_NAME}")

#add_subdirectory(toolchains)
install(DIRECTORY toolchains DESTINATION share/saga
	FILES_MATCHING PATTERN "*.xml")
