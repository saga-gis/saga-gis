#.rst:
# FindQhull
# --------
#
# Find libqhull
#
# Find the Qhull library
#
# ::
#
#   This module defines the following variables:
#      Qhull_FOUND       - True if Qhull_INCLUDE_DIR & Qhull_LIBRARY are found
#      Qhull_LIBRARIES   - Set when Qhull_LIBRARY is found
#      Qhull_INCLUDE_DIRS - Set when Qhull_INCLUDE_DIR is found
#
#
#
# ::
#
#      Qhull_INCLUDE_DIR - where to find qhull/Qhull.h, etc.
#      Qhull_LIBRARY     - the qhull library

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

find_path(Qhull_INCLUDE_DIR NAMES qhull/qhull.h libqhull_r/libqhull_r.h
          DOC "The Qhull include directory"
)

find_library(Qhull_LIBRARY NAMES qhull
          DOC "The Qhull library"
)

# handle the QUIETLY and REQUIRED arguments and set Qhull_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Qhull
	REQUIRED_VARS Qhull_LIBRARY Qhull_INCLUDE_DIR)

if(Qhull_FOUND)
  set( Qhull_LIBRARIES ${Qhull_LIBRARY} )
  set( Qhull_INCLUDE_DIRS ${Qhull_INCLUDE_DIR} )
endif()

mark_as_advanced(Qhull_INCLUDE_DIR Qhull_LIBRARY)
