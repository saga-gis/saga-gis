option(WITH_EXIV2 "Check to create build targets for Exiv2 GPS Tag extraction" ON)

if(WITH_TOOLS)
	project(io_grid_image)

	# load common tool template
	include(../../CMakePluginTemplate.cmake)

	# include directories of wxWidgets are already present
	find_package(wxWidgets REQUIRED QUIET)
	include("${wxWidgets_USE_FILE}")

	target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES} )
	
	if(WITH_EXIV2)
		if(MSVC AND DEPS_FROM_ENVARS)
			set(LIB4SAGA $ENV{EXIV2})
			find_path   (EXIV2_INCLUDE_DIR NAMES exiv2/exif.hpp PATHS "${LIB4SAGA}/include")
			find_library(EXIV2_LIBRARY     NAMES exiv2          PATHS "${LIB4SAGA}/lib")
			include(FindPackageHandleStandardArgs)
			FIND_PACKAGE_HANDLE_STANDARD_ARGS(EXIV2 REQUIRED_VARS EXIV2_LIBRARY EXIV2_INCLUDE_DIR)

			if( EXISTS "${EXIV2_INCLUDE_DIR}" AND EXISTS "${EXIV2_LIBRARY}")
				target_include_directories(${PROJECT_NAME} PUBLIC ${EXIV2_INCLUDE_DIR})
				target_link_libraries     (${PROJECT_NAME}        ${EXIV2_LIBRARY})
				target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_EXIV2)

				mark_as_advanced(EXIV2_INCLUDE_DIR EXIV2_LIBRARY)
			endif()

		else() # non-msw
			find_package(exiv2 QUIET)
 			if(exiv2_FOUND)
				target_link_libraries(${PROJECT_NAME} Exiv2::exiv2lib)
				target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_EXIV2)
			endif()
		endif()
	endif()
endif()
