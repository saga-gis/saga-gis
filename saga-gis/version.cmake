# The version number.
set (SAGA_VERSION_MAJOR 7)
set (SAGA_VERSION_MINOR 10)
set (SAGA_VERSION_PATCH 0)

# get git commit hash (or dirty if git is not installed)

# load helper scripts
include(shared_cmake/scripts/load_git_variables.cmake)
include(shared_cmake/scripts/gen_version_string.cmake)

# load git variables
load_git_variables(GIT_BRANCH GIT_COUNT GIT_COMMIT)

# define version strings
gen_version_string(SAGA_VERSION_STRING SAGA_VERSION_FULL_STRING
	${SAGA_VERSION_MAJOR} ${SAGA_VERSION_MINOR} ${SAGA_VERSION_PATCH}
	${GIT_COUNT} ${GIT_COMMIT})

message("GIT_BRANCH:         ${GIT_BRANCH}")
message("VERSON_STRING:      ${SAGA_VERSION_STRING}")
message("VERSON_FULL_STRING: ${SAGA_VERSION_FULL_STRING}")
