#.rst:
# Finddxflib
# --------
#
# Find dxflib
#
# Find the dxflib library
#
# ::
#
#   This module defines the following variables:
#      dxflib_FOUND       - True if dxflib_INCLUDE_DIR & dxflib_LIBRARY are found
#      dxflib_LIBRARIES   - Set when dxflib_LIBRARY is found
#      dxflib_INCLUDE_DIRS - Set when dxflib_INCLUDE_DIR is found
#
#
#
# ::
#
#      dxflib_INCLUDE_DIR - where to find hpdf.h, etc.
#      dxflib_LIBRARY     - the dxflib library
#      dxflib_VERSION_STRING - the version of dxflib found (since CMake 2.8.8)

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

find_package(PkgConfig)
pkg_check_modules(PC_ QUIET dxflib)

find_path(dxflib_INCLUDE_DIR NAMES dxflib/dl_dxf.h
	HINTS ${PC_DXFLIB_INCLUDEDIR} ${PC_DXFLIB_INCLUDE_DIRS}
	DOC "The dxflib include directory"
)

find_library(dxflib_LIBRARY NAMES dxflib
	HINTS ${PC_DXFLIB_LIBDIR} ${PC_DXFLIB_LIBRARY_DIRS}
	DOC "The dxflib library"
)

#define DL_VERSION     "2.5.0.0"
if(dxflib_INCLUDE_DIR AND EXISTS "${dxflib_INCLUDE_DIR}/dxflib/dl_dxf.h")
  file(STRINGS "${dxflib_INCLUDE_DIR}/dxflib/dl_dxf.h" dxflib_version_str REGEX "^#define[\t ]+DF_VERSION[\t ]+\".*\"")

  string(REGEX REPLACE "^.*DF_VERSION[\t ]+\"([^\"]*)\".*$" "\\1" dxflib_VERSION_STRING "${dxflib_version_str}")
  unset(dxflib_version_str)
endif()

# handle the QUIETLY and REQUIRED arguments and set dxflib_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dxflib
                                  REQUIRED_VARS dxflib_LIBRARY dxflib_INCLUDE_DIR
                                  VERSION_VAR dxflib_VERSION_STRING)

if(dxflib_FOUND)
  set( dxflib_LIBRARIES ${dxflib_LIBRARY} )
  set( dxflib_INCLUDE_DIRS ${dxflib_INCLUDE_DIR} )
endif()

mark_as_advanced(dxflib_INCLUDE_DIR dxflib_LIBRARY)
