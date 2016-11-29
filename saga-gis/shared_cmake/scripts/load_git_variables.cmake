# read git variables into given arguments
function(load_git_variables
		GIT_BRANCH_OUT
		GIT_COUNT_OUT
		GIT_COMMIT_HASH_OUT)
	find_package(Git)
	if (GIT_FOUND)
		# Get the latest abbreviated commit hash of the working branch
		execute_process(
			COMMAND ${GIT_EXECUTABLE} log -1 --format=%h
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			OUTPUT_VARIABLE GIT_COMMIT_HASH
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)

		# Get the last tag (if any)
		execute_process(
			COMMAND ${GIT_EXECUTABLE} describe --tags --abbrev=0
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			OUTPUT_VARIABLE GIT_LAST_TAG
			OUTPUT_STRIP_TRAILING_WHITESPACE
			ERROR_QUIET
		)

		# count the number of commits since last tag or since the beginning
		if ("${GIT_LAST_TAG}" STREQUAL "")
			set(GIT_LAST_TAG_FLAG "")
		else()
			set(GIT_LAST_TAG_FLAG "^${GIT_LAST_TAG}")
		endif()
		execute_process(
			COMMAND ${GIT_EXECUTABLE} rev-list --count HEAD ${GIT_LAST_TAG_FLAG}
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			OUTPUT_VARIABLE GIT_COUNT
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)

		# determine the current branch
		if (NOT GIT_COUNT)
			# we are currently building a tag
			# set the branch name to the tag name
			SET(GIT_BRANCH ${GIT_LAST_TAG})
		else()
			# we are building an arbitrary branch
			# extract the name of the branch the last commit has been made on, so this also works
			# in detached head state (which is the default on Gitlab CI)
			execute_process(
				COMMAND ${GIT_EXECUTABLE} name-rev --name-only HEAD
				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
				OUTPUT_VARIABLE GIT_BRANCH
				OUTPUT_STRIP_TRAILING_WHITESPACE
			)
			STRING(REGEX REPLACE "^remotes/origin/" "" GIT_BRANCH ${GIT_BRANCH})
		endif()

	else(GIT_FOUND)
		message("Git was not found")
		set(GIT_BRANCH "detached")
		set(GIT_COUNT "~")
		set(GIT_COMMIT_HASH "dirty")
	endif(GIT_FOUND)

	# fill return values
	set(${GIT_BRANCH_OUT} ${GIT_BRANCH}  PARENT_SCOPE)
	set(${GIT_COUNT_OUT} ${GIT_COUNT} PARENT_SCOPE)
	set(${GIT_COMMIT_HASH_OUT} ${GIT_COMMIT_HASH} PARENT_SCOPE)
endfunction()
