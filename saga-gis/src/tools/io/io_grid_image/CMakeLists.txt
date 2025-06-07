option(WITH_EXIV2 "Check to create build targets for Exiv2 GPS Tag extraction" ON)

if(WITH_TOOLS)
	project(io_grid_image)

	# load common tool template
	include(../../CMakePluginTemplate.cmake)

	# include directories of wxWidgets are already present
	find_package(wxWidgets REQUIRED QUIET)
	include("${wxWidgets_USE_FILE}")

	target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES} )
	
	if(WITH_EXIV2 )
		find_package(exiv2 QUIET )
		
 		if(exiv2_FOUND)
			target_link_libraries(${PROJECT_NAME} Exiv2::exiv2lib )
			target_compile_definitions(${PROJECT_NAME} PRIVATE HAVE_EXIV2)
		endif()
	endif()
endif()
