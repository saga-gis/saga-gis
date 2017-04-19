# Generate version information for Windows binaries
function(gen_win_version_resource_ex	PROJECT_NAME RESOURCE_FILE_PATH
										FILE_VERSION_1_IN FILE_VERSION_2_IN FILE_VERSION_3_IN FILE_VERSION_4_IN FILE_VERSION_STRING
										PROD_VERSION_1_IN PROD_VERSION_2_IN PROD_VERSION_3_IN PROD_VERSION_4_IN PROD_VERSION_STRING)
	
	# file version information
	if("${FILE_VERSION_1_IN}" STREQUAL "")
		message(FATAL_ERROR "At least major file version number required")
	endif()
	set(RC_FILE_VERSION_NUMBER "${FILE_VERSION_1_IN}")
	if(NOT "${FILE_VERSION_2_IN}" STREQUAL "")
		set(RC_FILE_VERSION_NUMBER "${RC_FILE_VERSION_NUMBER},${FILE_VERSION_2_IN}")
	endif()
	if(NOT "${FILE_VERSION_3_IN}" STREQUAL "")
		set(RC_FILE_VERSION_NUMBER "${RC_FILE_VERSION_NUMBER},${FILE_VERSION_3_IN}")
	endif()
	if(NOT "${FILE_VERSION_4_IN}" STREQUAL "")
		set(RC_FILE_VERSION_NUMBER "${RC_FILE_VERSION_NUMBER},${FILE_VERSION_4_IN}")
	endif()
	
	# product version information
	if("${PROD_VERSION_1_IN}" STREQUAL "")
		message(FATAL_ERROR "At least major product version number required")
	endif()
	set(RC_PRODUCT_VERSION_NUMBER "${PROD_VERSION_1_IN}")
	if(NOT "${PROD_VERSION_2_IN}" STREQUAL "")
		set(RC_PRODUCT_VERSION_NUMBER "${RC_PRODUCT_VERSION_NUMBER},${PROD_VERSION_2_IN}")
	endif()
	if(NOT "${PROD_VERSION_3_IN}" STREQUAL "")
		set(RC_PRODUCT_VERSION_NUMBER "${RC_PRODUCT_VERSION_NUMBER},${PROD_VERSION_3_IN}")
	endif()
	if(NOT "${PROD_VERSION_4_IN}" STREQUAL "")
		set(RC_PRODUCT_VERSION_NUMBER "${RC_PRODUCT_VERSION_NUMBER},${PROD_VERSION_4_IN}")
	endif()
	
	set(RC_FILE_VERSION "${FILE_VERSION_STRING}")
	set(RC_PRODUCT_VERSION "${PROD_VERSION_STRING}")
	set(RC_PRODUCT_NAME ${PROJECT_NAME})
	set(RC_INTERNAL_NAME ${RC_PRODUCT_NAME})
	set(RC_COMPANY_NAME "Riegl LMS")
	set(RC_FILE_DESCRIPTION ${PROJECT_NAME})
	set(RC_COPYRIGHT ${RC_COMPANY_NAME})

	# configure a resource file to pass some of the CMake settings to msvc
	configure_file("${CMAKE_SOURCE_DIR}/shared_cmake/scripts/windows_version_info.rc.in" "${RESOURCE_FILE_PATH}" @ONLY)
endfunction()

# Convenience function that copies all file information to product information
function(gen_win_version_resource	PROJECT_NAME RESOURCE_FILE_PATH
										FILE_VERSION_1_IN FILE_VERSION_2_IN FILE_VERSION_3_IN FILE_VERSION_4_IN FILE_VERSION_STRING)
	gen_win_version_resource_ex("${PROJECT_NAME}" "${RESOURCE_FILE_PATH}"
								"${FILE_VERSION_1_IN}" "${FILE_VERSION_2_IN}"
								"${FILE_VERSION_3_IN}" "${FILE_VERSION_4_IN}" 
								"${FILE_VERSION_STRING}"
								# Copy file version info to product version info
								"${FILE_VERSION_1_IN}" "${FILE_VERSION_2_IN}"
								"${FILE_VERSION_3_IN}" "${FILE_VERSION_4_IN}" 
								"${FILE_VERSION_STRING}")
endfunction()
