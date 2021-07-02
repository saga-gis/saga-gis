#########################################
# Find the Haru PDF (hpdf) library
#
# HPDF_FOUND   - true if HPDF_INCLUDE and HPDF_LIBRARY are found
# HPDF_INCLUDE - where to find the HPDF headers
# HPDF_LIBRARY - the HPDF library
# HPDF_VERSION - the HPDF version string
#
#########################################

if("${CMAKE_GENERATOR_PLATFORM}" STREQUAL "Win32")
	set(LIB4SAGA $ENV{HARU_32})
else()
	set(LIB4SAGA $ENV{HARU})
endif()

find_path   (HPDF_INCLUDE NAMES hpdf.h       PATHS "${LIB4SAGA}/include" DOC "The Haru PDF (hpdf) include directory")
find_library(HPDF_LIBRARY NAMES hpdf libhpdf PATHS "${LIB4SAGA}"         DOC "The Haru PDF (hpdf) library")

if(HPDF_INCLUDE AND EXISTS "${HPDF_INCLUDE}/hpdf_version.h")
	file(STRINGS "${HPDF_INCLUDE}/hpdf_version.h" hpdf_version_str REGEX "^#define[\t ]+HPDF_VERSION_TEXT[\t ]+\".*\"")
	string(REGEX REPLACE "^.*HPDF_VERSION_TEXT[\t ]+\"([^\"]*)\".*$" "\\1" HPDF_VERSION_STRING "${hpdf_version_str}")
	unset(hpdf_version_str)
endif()

# handle the QUIETLY and REQUIRED arguments and set HPDF_FOUND to TRUE if all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(HPDF
	REQUIRED_VARS HPDF_LIBRARY HPDF_INCLUDE
	VERSION_VAR HPDF_VERSION_STRING
)

mark_as_advanced(HPDF_INCLUDE HPDF_LIBRARY)
