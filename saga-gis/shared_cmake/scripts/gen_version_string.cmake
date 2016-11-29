# Generate VERSION and VERSION_FULL strings
# All input-variables except MAJOR can be ""
function(gen_version_string VERSION_OUT VERSION_FULL_OUT MAJOR_IN MINOR_IN PATCH_IN COUNT_IN COMMIT_IN)

	# <MAJOR>.<MINOR>.<PATCH>
	set(VERSION ${MAJOR_IN})
	if(NOT ${MINOR_IN} STREQUAL "")
		set(VERSION ${VERSION}.${MINOR_IN})
	endif()
	if(NOT ${PATCH_IN} STREQUAL "")
		set(VERSION ${VERSION}.${PATCH_IN})
	endif()

	# <VERSION>-<COUNT>+COMMIT
	set(VERSION_FULL ${VERSION})
	if(NOT ${COUNT_IN} STREQUAL "")
		set(VERSION_FULL ${VERSION_FULL}-${COUNT_IN})
	endif()
	if(NOT ${COMMIT_IN} STREQUAL "")
		set(VERSION_FULL ${VERSION_FULL}+${COMMIT_IN})
	endif()
	if(NOT "${ARGN}" STREQUAL "")
		foreach(arg ${ARGN})
			set(VERSION_FULL ${VERSION_FULL}+${arg})
		endforeach()
	endif()

	# write to output variables
	set(${VERSION_OUT} ${VERSION} PARENT_SCOPE)
	set(${VERSION_FULL_OUT} ${VERSION_FULL} PARENT_SCOPE)
endfunction()
