message(STATUS "subfolder: ${PROJECT_NAME}/man")

install(
	FILES saga_cmd.1
	DESTINATION share/man/man1
)
