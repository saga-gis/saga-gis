#########################################
# Find the VIGRA library
#
# VIGRA_FOUND         - true if VIGRA_FFTW3_INCLUDE, VIGRA_FFTW3_LIBRARY, VIGRA_INCLUDE and VIGRA_LIBRARY are found
# VIGRA_INCLUDES      - where to find VIGRA and fftw3 headers
# VIGRA_LIBRARIES     - the VIGRA and fftw3 libraries
#
#########################################

find_path   (VIGRA_FFTW3_INCLUDE NAMES fftw3.h            DOC "fftw3 include directory")
find_library(VIGRA_FFTW3_LIBRARY NAMES fftw3              DOC "fftw3 library")
find_path   (VIGRA_INCLUDE       NAMES vigra/stdimage.hxx DOC "VIGRA include directory")
find_library(VIGRA_LIBRARY       NAMES vigraimpex         DOC "VIGRA library")

if(MSVC)
	find_path(VIGRA_HDF5_INCLUDE   NAMES hdf5.h   DOC "[optional] hdf5 include directory")
	find_path(VIGRA_HDF5_LIBRARIES NAMES hdf5.lib DOC "[optional] hdf5 libraries directory")
endif()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(VIGRA REQUIRED_VARS VIGRA_FFTW3_INCLUDE VIGRA_FFTW3_LIBRARY VIGRA_INCLUDE VIGRA_LIBRARY)

if(VIGRA_FOUND)
	set(VIGRA_INCLUDES  ${VIGRA_FFTW3_INCLUDE} ${VIGRA_INCLUDE})
	set(VIGRA_LIBRARIES ${VIGRA_FFTW3_LIBRARY} ${VIGRA_LIBRARY})

	if(MSVC)
		if( EXISTS ${VIGRA_HDF5_INCLUDE}/hdf5.h
		AND EXISTS ${VIGRA_HDF5_LIBRARIES}/hdf5.lib
		AND EXISTS ${VIGRA_HDF5_LIBRARIES}/hdf5_hl.lib)
			set(VIGRA_INCLUDES  ${VIGRA_FFTW3_INCLUDE} ${VIGRA_INCLUDE} ${VIGRA_HDF5_INCLUDE})
			set(VIGRA_LIBRARIES ${VIGRA_FFTW3_LIBRARY} ${VIGRA_LIBRARY} ${VIGRA_HDF5_LIBRARIES}/hdf5.lib ${VIGRA_HDF5_LIBRARIES}/hdf5_hl.lib)
			add_definitions(-DWITH_HDF5)
			message(STATUS "VIGRA with HDF5 support")
		endif()
	else()
		find_package(HDF5)	# on Ubuntu package libhdf5-dev
		if(HDF5_FOUND)
			set(VIGRA_INCLUDES  ${VIGRA_FFTW3_INCLUDE} ${VIGRA_INCLUDE} ${HDF5_INCLUDE_DIRS})
			set(VIGRA_LIBRARIES ${VIGRA_FFTW3_LIBRARY} ${VIGRA_LIBRARY} ${HDF5_LIBRARIES})
			add_definitions(-DWITH_HDF5)
			message(STATUS "VIGRA with HDF5 support")
		endif()
	endif()
endif()

mark_as_advanced(VIGRA_FFTW3_INCLUDE VIGRA_FFTW3_LIBRARY VIGRA_INCLUDE VIGRA_LIBRARY)
