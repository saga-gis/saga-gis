#########################################
# Find the gdal library
#
# gdal_FOUND       - true if gdal_INCLUDE_DIR and gdal_LIBRARY are found
# gdal_INCLUDE_DIR - where to find gdal.h, etc.
# gdal_LIBRARY     - the gdal library
#
#########################################

find_path   (gdal_INCLUDE_DIR NAMES gdal.h DOC "The GDAL include directory")
find_library(gdal_LIBRARY NAMES     gdal   DOC "The GDAL library")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(gdal REQUIRED_VARS gdal_LIBRARY gdal_INCLUDE_DIR)

mark_as_advanced(gdal_INCLUDE_DIR gdal_LIBRARY)
