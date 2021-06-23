#########################################
# Find the ViGra library
#
# VIGRA_FOUND       - true if VIGRA_INCLUDE_DIR and VIGRA_LIBRARY are found
# VIGRA_INCLUDE_DIR - where to find vigra headers, etc.
# VIGRA_LIBRARY     - the ViGra library
#
#########################################

find_path   (VIGRA_INCLUDE_DIR NAMES vigra/stdimage.hxx DOC "ViGra include directory")
find_library(VIGRA_LIBRARY     NAMES vigraimpex         DOC "The ViGra library")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(vigra REQUIRED_VARS VIGRA_LIBRARY VIGRA_INCLUDE_DIR)

if(VIGRA_FOUND)
	set(VIGRA_LIBRARIES    ${VIGRA_LIBRARY} )
	set(VIGRA_INCLUDE_DIRS ${VIGRA_INCLUDE_DIR} )
endif()

mark_as_advanced(VIGRA_INCLUDE_DIR VIGRA_LIBRARY)
