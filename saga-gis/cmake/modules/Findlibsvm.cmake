#.rst:
# Findlibsvm
# --------
#
# Find libsvm
#
# Find LIBSVM -- A Library for Support Vector Machines
#
# ::
#
#   This module defines the following variables:
#      libsvm_FOUND       - True if libsvm_INCLUDE_DIR & libsvm_LIBRARY are found
#      libsvm_LIBRARIES   - Set when libsvm_LIBRARY is found
#      libsvm_INCLUDE_DIRS - Set when libsvm_INCLUDE_DIR is found
#
#
#
# ::
#
#      libsvm_INCLUDE_DIR - where to find asoundlib.h, etc.
#      libsvm_LIBRARY     - the asound library
#      libsvm_VERSION_STRING - the version of alsa found (since CMake 2.8.8)

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

find_path(libsvm_INCLUDE_DIR NAMES libsvm/svm.h
          DOC "The libsvm include directory"
)

find_library(libsvm_LIBRARY NAMES svm
          DOC "The libsvm library"
)

# handle the QUIETLY and REQUIRED arguments and set libsvm_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(libsvm
	REQUIRED_VARS libsvm_LIBRARY libsvm_INCLUDE_DIR)

if(libsvm_FOUND)
  set( libsvm_LIBRARIES ${libsvm_LIBRARY} )
  set( libsvm_INCLUDE_DIRS ${libsvm_INCLUDE_DIR} )
endif()

mark_as_advanced(libsvm_INCLUDE_DIR libsvm_LIBRARY)
