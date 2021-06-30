if(WITH_TOOLS)
	project(garden_webservices)
	include(../../CMakePluginTemplate.cmake)	# load common tool template

	#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#
	if(MSVC AND DEPS_FROM_ENVARS)
		find_path(CURL_INCLUDE_DIRS NAMES curl/curl.h PATHS "$ENV{GDAL}/include")
		find_library(CURL_LIBRARIES NAMES curl libcurl_imp PATHS "$ENV{GDAL}/lib")
		include(FindPackageHandleStandardArgs)
		FIND_PACKAGE_HANDLE_STANDARD_ARGS(CURL REQUIRED_VARS CURL_LIBRARIES CURL_INCLUDE_DIRS)
		mark_as_advanced(CURL_INCLUDE_DIRS CURL_LIBRARIES)
	endif()
	#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#

	if(NOT CURL_FOUND)
		find_package(CURL QUIET)	# Ubuntu package libcurl-dev
	endif()
	if(CURL_FOUND)
		target_include_directories(${PROJECT_NAME} PRIVATE ${CURL_INCLUDE_DIRS})
		target_link_libraries(${PROJECT_NAME} ${CURL_LIBRARIES})
		add_definitions(/DHAVE_LIBCURL)
	#	message("[${PROJECT_NAME}] with curl, include: ${CURL_INCLUDE_DIR}")
	#	message("[${PROJECT_NAME}] with curl, library: ${CURL_LIBRARIES}")
	endif()

	find_package(wxWidgets REQUIRED QUIET)
	include("${wxWidgets_USE_FILE}")
	target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
endif()
