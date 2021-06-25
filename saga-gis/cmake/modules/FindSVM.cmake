#########################################
# Find the SVM library - A Library for Support Vector Machines
#
# SVM_FOUND   - true if SVM_INCLUDE and SVM_LIBRARY are found
# SVM_INCLUDE - where to find SVM headers
# SVM_LIBRARY - the SVM library
#
#########################################

find_path   (SVM_INCLUDE NAMES libsvm/svm.h DOC "libsvm include directory")
find_library(SVM_LIBRARY NAMES svm          DOC "libsvm library")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(svm REQUIRED_VARS SVM_LIBRARY SVM_INCLUDE)

mark_as_advanced(SVM_INCLUDE SVM_LIBRARY)
