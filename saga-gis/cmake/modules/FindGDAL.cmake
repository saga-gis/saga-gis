#########################################
# Find the GDAL library
#
# GDAL_FOUND       - true if GDAL_INCLUDE_DIR and GDAL_LIBRARY are found
# GDAL_INCLUDE_DIR - where to find gdal.h, etc.
# GDAL_LIBRARY     - the GDAL library
#
#########################################

find_path   (GDAL_INCLUDE_DIR NAMES gdal.h DOC "The GDAL include directory")
find_library(GDAL_LIBRARY NAMES     gdal   DOC "The GDAL library")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(gdal REQUIRED_VARS GDAL_LIBRARY GDAL_INCLUDE_DIR)

mark_as_advanced(GDAL_INCLUDE_DIR GDAL_LIBRARY)
