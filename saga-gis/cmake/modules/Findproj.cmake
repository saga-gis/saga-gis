#.rst:
# Findproj
# --------
#
# Find libproj
#
# Find the proj library
#
# ::
#
#   This module defines the following variables:
#      proj_FOUND       - True if proj_INCLUDE_DIR & proj_LIBRARY are found
#      proj_LIBRARIES   - Set when proj_LIBRARY is found
#      proj_INCLUDE_DIRS - Set when proj_INCLUDE_DIR is found
#
#
#
# ::
#
#      proj_INCLUDE_DIR - where to find projects.h, etc.
#      proj_LIBRARY     - the proj library

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

find_path(proj_INCLUDE_DIR NAMES projects.h
          DOC "The proj include directory"
)

find_library(proj_LIBRARY NAMES proj
          DOC "The proj library"
)

# handle the QUIETLY and REQUIRED arguments and set proj_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(proj
	REQUIRED_VARS proj_LIBRARY proj_INCLUDE_DIR)

if(proj_FOUND)
  set( proj_LIBRARIES ${proj_LIBRARY} )
  set( proj_INCLUDE_DIRS ${proj_INCLUDE_DIR} )
endif()

mark_as_advanced(proj_INCLUDE_DIR proj_LIBRARY)
