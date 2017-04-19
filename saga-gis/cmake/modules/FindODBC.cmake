#.rst:
# FindODBC
# --------
#
# Find libodbc
#
# Find the Open Database Connectivity (ODBC) library
#
# ::
#
#   This module defines the following variables:
#      ODBC_FOUND       - True if ODBC_INCLUDE_DIR & ODBC_LIBRARY are found
#      ODBC_LIBRARIES   - Set when ODBC_LIBRARY is found
#      ODBC_INCLUDE_DIRS - Set when ODBC_INCLUDE_DIR is found
#
#
#
# ::
#
#      ODBC_INCLUDE_DIR - where to find asoundlib.h, etc.
#      ODBC_LIBRARY     - the asound library
#      ODBC_VERSION_STRING - the version of alsa found (since CMake 2.8.8)

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

find_path(ODBC_INCLUDE_DIR NAMES odbcinst.h
          DOC "The Open Database Connectivity (ODBC) include directory"
)

find_library(ODBC_LIBRARY NAMES odbc
          DOC "The Open Database Connectivity (ODBC) library"
)
# TODO: libodbcinst.so and libodbccr.so also exist

# handle the QUIETLY and REQUIRED arguments and set ODBC_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ODBC
	REQUIRED_VARS ODBC_LIBRARY ODBC_INCLUDE_DIR)

if(ODBC_FOUND)
  set( ODBC_LIBRARIES ${ODBC_LIBRARY} )
  set( ODBC_INCLUDE_DIRS ${ODBC_INCLUDE_DIR} )
endif()

mark_as_advanced(ODBC_INCLUDE_DIR ODBC_LIBRARY)
