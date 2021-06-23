#########################################
# Find the proj library
#
# PROJ_FOUND       - true if PROJ_INCLUDE_DIR and PROJ_LIBRARY are found
# PROJ_INCLUDE_DIR - where to find projects.h, etc.
# PROJ_LIBRARY     - the proj library
#
#########################################

find_path   (PROJ_INCLUDE_DIR NAMES projects.h DOC "The proj include directory")
find_library(PROJ_LIBRARY     NAMES proj       DOC "The proj library")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(proj REQUIRED_VARS PROJ_LIBRARY PROJ_INCLUDE_DIR)

mark_as_advanced(PROJ_INCLUDE_DIR PROJ_LIBRARY)
