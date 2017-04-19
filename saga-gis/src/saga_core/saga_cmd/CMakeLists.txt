project(saga_cmd)
message(STATUS "project: ${PROJECT_NAME}")

# define sources
set(SAGA_CMD_SOURCES
	callback.cpp
	callback.h
	config.cpp
	config.h
	tool.cpp
	tool.h
	saga_cmd.cpp
)

add_executable(saga_cmd
	${SAGA_CMD_SOURCES}
)

# link saga_api
target_link_libraries(saga_cmd saga_api)

# find and use wxWidgeds
find_package(wxWidgets COMPONENTS base REQUIRED)
target_link_libraries(saga_cmd ${wxWidgets_LIBRARIES})

target_compile_definitions(saga_cmd PRIVATE -D${MLIBPATH})
target_compile_definitions(saga_cmd PRIVATE -D${MSHAREPATH})

# add man page
add_subdirectory(man)

# where to install the binary
install(TARGETS saga_cmd DESTINATION bin)
