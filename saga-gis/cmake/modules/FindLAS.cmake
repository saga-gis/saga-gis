#########################################
# Find the LAS library
#
# LAS_FOUND   - true if LAS_INCLUDE and LAS_LIBRARY are found
# LAS_INCLUDE - where to find the LAS headers
# LAS_LIBRARY - the LAS library
# LAS_VERSION - the LAS version string
#
#########################################

find_path   (LAS_INCLUDE NAMES liblas/liblas.hpp PATHS "$ENV{LIBLAS}/include")
find_library(LAS_LIBRARY NAMES las liblas        PATHS "$ENV{LIBLAS}/lib")

#define LIBLAS_LIB_VERSION "1_6"
if(LAS_INCLUDE AND EXISTS "${LAS_INCLUDE}/liblas/version.hpp")
	file(STRINGS "${LAS_INCLUDE}/liblas/version.hpp" LAS_version_str REGEX "^#define[\t ]+LIBLAS_LIB_VERSION[\t ]+\".*\"")
	string(REGEX REPLACE "^.*LIBLAS_LIB_VERSION[\t ]+\"([^\"]*)\".*$" "\\1" LAS_VERSION_STRING "${LAS_version_str}")
	string(REPLACE "_" "." LAS_VERSION_STRING "${LAS_VERSION_STRING}")
	unset(LAS_version_str)
endif()

# handle the QUIETLY and REQUIRED arguments and set LAS_FOUND to TRUE if all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LAS
	REQUIRED_VARS LAS_LIBRARY LAS_INCLUDE
	VERSION_VAR LAS_VERSION_STRING
)

mark_as_advanced(LAS_INCLUDE LAS_LIBRARY)
