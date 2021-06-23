#########################################
# Find the OpenCV library
#
# OpenCV_FOUND        - true if GDAL_INCLUDE_DIR and GDAL_LIBRARY are found
# OpenCV_INCLUDE_DIRS - where to find the OpenCV headers
# OpenCV_LIBRARIES    - the OpenCV libraries
#
#########################################

find_path(OpenCV_INCLUDE_DIRS NAMES opencv2/opencv.hpp DOC "OpenCV include directory")
find_path(OpenCV_LIBRARIES    NAMES opencv_core        DOC "OpenCV libraries directory")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OpenCV REQUIRED_VARS OpenCV_LIBRARIES OpenCV_INCLUDE_DIRS)

mark_as_advanced(OpenCV_INCLUDE_DIRS OpenCV_LIBRARIES)
