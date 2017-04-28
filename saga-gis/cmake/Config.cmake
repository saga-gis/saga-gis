# translate some cmake variables to config.h variables
macro(Config)
	# define VERSION string
	if (SAGA_VERSION_STRING)
		set(VERSION ON)
	endif()

	configure_file(cmake/config.h.cmake config.h)
endmacro()
