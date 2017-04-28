#.rst:
# FindHPDF
# --------
#
# Find libhpdf
#
# Find the Haru PDF library (hpdf)
#
# ::
#
#   This module defines the following variables:
#      HPDF_FOUND       - True if HPDF_INCLUDE_DIR & HPDF_LIBRARY are found
#      HPDF_LIBRARIES   - Set when HPDF_LIBRARY is found
#      HPDF_INCLUDE_DIRS - Set when HPDF_INCLUDE_DIR is found
#
#
#
# ::
#
#      HPDF_INCLUDE_DIR - where to find hpdf.h, etc.
#      HPDF_LIBRARY     - the hpdf library
#      HPDF_VERSION_STRING - the version of Haru PDF found (since CMake 2.8.8)

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

find_path(HPDF_INCLUDE_DIR NAMES hpdf.h
          DOC "The Haru PDF (hpdf) include directory"
)

find_library(HPDF_LIBRARY NAMES hpdf
          DOC "The Haru PDF (hpdf) library"
)

if(HPDF_INCLUDE_DIR AND EXISTS "${HPDF_INCLUDE_DIR}/hpdf_version.h")
  file(STRINGS "${HPDF_INCLUDE_DIR}/hpdf_version.h" hpdf_version_str REGEX "^#define[\t ]+HPDF_VERSION_TEXT[\t ]+\".*\"")

  string(REGEX REPLACE "^.*HPDF_VERSION_TEXT[\t ]+\"([^\"]*)\".*$" "\\1" HPDF_VERSION_STRING "${hpdf_version_str}")
  unset(hpdf_version_str)
endif()

# handle the QUIETLY and REQUIRED arguments and set HPDF_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(HPDF
                                  REQUIRED_VARS HPDF_LIBRARY HPDF_INCLUDE_DIR
                                  VERSION_VAR HPDF_VERSION_STRING)

if(HPDF_FOUND)
  set( HPDF_LIBRARIES ${HPDF_LIBRARY} )
  set( HPDF_INCLUDE_DIRS ${HPDF_INCLUDE_DIR} )
endif()

mark_as_advanced(HPDF_INCLUDE_DIR HPDF_LIBRARY)
