#.rst:
# Findgeotrans
# --------
#
# Find libgeotrans
#
# Find the geotrans library
#
# ::
#
#   This module defines the following variables:
#      geotrans_FOUND       - True if geotrans_INCLUDE_DIR & geotrans_LIBRARY are found
#      geotrans_LIBRARIES   - Set when geotrans_LIBRARY is found
#      geotrans_INCLUDE_DIRS - Set when geotrans_INCLUDE_DIR is found
#
#
#
# ::
#
#      geotrans_INCLUDE_DIR - where to find geotrans/geotrans.h, etc.
#      geotrans_LIBRARY     - the geotrans library

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

find_path(geotrans_INCLUDE_DIR NAMES geotrans/geotrans.h
          DOC "The geotrans include directory"
)

find_library(geotrans_LIBRARY NAMES geotrans
          DOC "The geotrans library"
)

# handle the QUIETLY and REQUIRED arguments and set geotrans_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(geotrans
	REQUIRED_VARS geotrans_LIBRARY geotrans_INCLUDE_DIR)

if(geotrans_FOUND)
  set( geotrans_LIBRARIES ${geotrans_LIBRARY} )
  set( geotrans_INCLUDE_DIRS ${geotrans_INCLUDE_DIR} )
endif()

mark_as_advanced(geotrans_INCLUDE_DIR geotrans_LIBRARY)
