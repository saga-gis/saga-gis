#########################################
# Find the curl library
#
# curl_FOUND       - true if curl_INCLUDE_DIR and curl_LIBRARY are found
# curl_INCLUDE_DIR - where to find curl.h, etc.
# curl_LIBRARY     - the curl library
#
#########################################

find_path   (curl_INCLUDE_DIR NAMES curl.h DOC "The curl include directory")
find_library(curl_LIBRARY NAMES     curl   DOC "The curl library")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(curl REQUIRED_VARS curl_LIBRARY curl_INCLUDE_DIR)

mark_as_advanced(curl_INCLUDE_DIR curl_LIBRARY)
