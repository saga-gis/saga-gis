#.rst:
# Findfftw3
# --------
#
# Find libfftw3
#
# Find the fftw3 library
#
# ::
#
#   This module defines the following variables:
#      fftw3_FOUND       - True if fftw3_INCLUDE_DIR & fftw3_LIBRARY are found
#      fftw3_LIBRARIES   - Set when fftw3_LIBRARY is found
#      fftw3_INCLUDE_DIRS - Set when fftw3_INCLUDE_DIR is found
#
#
#
# ::
#
#      fftw3_INCLUDE_DIR - where to find fftw3.h, etc.
#      fftw3_LIBRARY     - the fftw3 library

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

find_path(fftw3_INCLUDE_DIR NAMES fftw3.h
          DOC "The fftw3 include directory"
)

find_library(fftw3_LIBRARY NAMES fftw3
          DOC "The fftw3 library"
)

# handle the QUIETLY and REQUIRED arguments and set fftw3_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(fftw3
	REQUIRED_VARS fftw3_LIBRARY fftw3_INCLUDE_DIR)

if(fftw3_FOUND)
  set( fftw3_LIBRARIES ${fftw3_LIBRARY} )
  set( fftw3_INCLUDE_DIRS ${fftw3_INCLUDE_DIR} )
endif()

mark_as_advanced(fftw3_INCLUDE_DIR fftw3_LIBRARY)
