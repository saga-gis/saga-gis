#.rst:
# Findvigra
# --------
#
# Find libodbc
#
# Find the Open Database Connectivity (vigra) library
#
# ::
#
#   This module defines the following variables:
#      vigra_FOUND       - True if vigra_INCLUDE_DIR & vigra_LIBRARY are found
#      vigra_LIBRARIES   - Set when vigra_LIBRARY is found
#      vigra_INCLUDE_DIRS - Set when vigra_INCLUDE_DIR is found
#
#
#
# ::
#
#      vigra_INCLUDE_DIR - where to find vigra headers.
#      vigra_LIBRARY     - the vigraimpex library
#      vigra_VERSION_STRING - the version of vigra found (since CMake 2.8.8)

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

find_path(vigra_INCLUDE_DIR NAMES vigra/stdimage.hxx
          DOC "vigra include directory"
)

find_library(vigra_LIBRARY NAMES vigraimpex
          DOC "The vigra library"
)

# handle the QUIETLY and REQUIRED arguments and set vigra_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(vigra
	REQUIRED_VARS vigra_LIBRARY vigra_INCLUDE_DIR)

if(vigra_FOUND)
  set( vigra_LIBRARIES ${vigra_LIBRARY} )
  set( vigra_INCLUDE_DIRS ${vigra_INCLUDE_DIR} )
endif()

mark_as_advanced(vigra_INCLUDE_DIR vigra_LIBRARY)
