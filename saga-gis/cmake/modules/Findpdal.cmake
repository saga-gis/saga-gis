#########################################
# Find the PDAL library
#
# PDAL_FOUND       - true if PDAL_INCLUDE_DIR and PDAL_LIBRARY are found
# PDAL_INCLUDE_DIR - where to find pdal.h, etc.
# PDAL_LIB_CPP     - the PDAL CPP library
# PDAL_LIB_UTIL    - the PDAL Utils library
#
#########################################

find_path   (PDAL_INCLUDE_DIR NAMES pdal.h    DOC "The PDAL include directory")
find_library(PDAL_LIB_CPP     NAMES pdalcpp   DOC "The PDAL library")
find_library(PDAL_LIB_UTIL    NAMES pdal_util DOC "The PDAL library")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PDAL REQUIRED_VARS PDAL_LIB_CPP PDAL_INCLUDE_DIR)

mark_as_advanced(PDAL_INCLUDE_DIR PDAL_LIB_CPP PDAL_LIB_UTIL)
