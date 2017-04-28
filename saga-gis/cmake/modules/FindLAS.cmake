#.rst:
# FindLAS
# --------
#
# Find LAS
#
# Find the LAS library
#
# ::
#
#   This module defines the following variables:
#      LAS_FOUND       - True if LAS_INCLUDE_DIR & LAS_LIBRARY are found
#      LAS_LIBRARIES   - Set when LAS_LIBRARY is found
#      LAS_INCLUDE_DIRS - Set when LAS_INCLUDE_DIR is found
#
#
#
# ::
#
#      LAS_INCLUDE_DIR - where to find liblas/liblas.hpp, etc.
#      LAS_LIBRARY     - the LAS library
#      LAS_VERSION_STRING - the version of LAS found (since CMake 2.8.8)

#=============================================================================
# Copyright 2009-2011 Kitware, Inc.
# Copyright 2009-2011 Philip Lowman <philip@yhbt.com>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

find_path(LAS_INCLUDE_DIR NAMES liblas/liblas.hpp
          DOC "The LAS include directory"
)

find_library(LAS_LIBRARY NAMES las
          DOC "The LAS library"
)
#define LIBLAS_LIB_VERSION "1_6"
if(LAS_INCLUDE_DIR AND EXISTS "${LAS_INCLUDE_DIR}/liblas/version.hpp")
  file(STRINGS "${LAS_INCLUDE_DIR}/liblas/version.hpp" LAS_version_str REGEX "^#define[\t ]+LIBLAS_LIB_VERSION[\t ]+\".*\"")

  string(REGEX REPLACE "^.*LIBLAS_LIB_VERSION[\t ]+\"([^\"]*)\".*$" "\\1" LAS_VERSION_STRING "${LAS_version_str}")
  string(REPLACE "_" "." LAS_VERSION_STRING "${LAS_VERSION_STRING}")
  unset(LAS_version_str)
endif()

# handle the QUIETLY and REQUIRED arguments and set LAS_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LAS
                                  REQUIRED_VARS LAS_LIBRARY LAS_INCLUDE_DIR
                                  VERSION_VAR LAS_VERSION_STRING)

if(LAS_FOUND)
  set( LAS_LIBRARIES ${LAS_LIBRARY} )
  set( LAS_INCLUDE_DIRS ${LAS_INCLUDE_DIR} )
endif()

mark_as_advanced(LAS_INCLUDE_DIR LAS_LIBRARY)
