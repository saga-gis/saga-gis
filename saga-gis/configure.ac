
#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(saga, 7.7.0, BUG-REPORT-ADDRESS)
AC_CONFIG_SRCDIR([src/saga_core/saga_gui/wksp_data_menu_files.cpp])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign tar-ustar subdir-objects])


# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL

# Checks for libraries.
AC_CHECK_LIB([opencv_core], [cvGetRows], CVFOUND=1,,)
AC_CHECK_LIB([geotrans], [Set_Mercator_Coordinates])
AC_CHECK_LIB([fftw3],[fftw_execute],FFTWFOUND=1,)
AC_CHECK_LIB([las], [LAS_GetVersion], LASFOUND=1,,)
AC_CHECK_LIB([pdal], [versionString], PDALFOUND=1,,)
AC_CHECK_LIB(hpdf, HPDF_Pages_New, HARUFOUND=1,,)
AC_CHECK_LIB([odbc], [SQLFetch], ODBCFOUND=1,[ODBC \(libodbc\) library not found])
# PROJ4 has projects.h and old API function:
AC_CHECK_HEADER([projects.h],[AC_CHECK_LIB([proj], [pj_get_release], PROJ4FOUND=1,,)])
# PROJ5 still has projects.h but also new proj.h and new API function:
AC_CHECK_HEADER([projects.h],[AC_CHECK_HEADER([proj.h],[AC_CHECK_LIB([proj], [proj_info], PROJ5FOUND=1,,)])])
# PROJ6 has dropped projects.h in favour of proj.h and has new API function:
AC_CHECK_HEADER([proj.h],[AC_CHECK_LIB([proj], [proj_info], PROJ6FOUND=1,,)])
AC_CHECK_LIB([vigraimpex], [VIGRA_RGBE_ReadPixels_Raw],VIGRAFOUND=1,)
AC_CHECK_LIB([gomp], [omp_get_num_threads], GOMPFOUND=1,,)
AC_CHECK_HEADER([libsvm/svm.h],[AC_CHECK_LIB([svm], [svm_get_svm_type], SVMFOUND=1,,)])
PKG_CHECK_MODULES([DXFLIB], [dxflib], [LIBDXFFOUND=1],[LIBDXFFOUND=0])
PKG_CHECK_MODULES([HDF5],[hdf5],LIBHDF5FOUND=1,LIBHDF5FOUND=0)
LIBCURL_CHECK_CONFIG([yes], [7.7.2],,)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([float.h malloc.h memory.h stdlib.h string.h unistd.h])
AC_CHECK_HEADERS([wchar.h])
# todo: check for headers of the libraries above

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_STRUCT_TM

# Checks for platform characteristics.
AC_C_BIGENDIAN

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_REALLOC
AC_FUNC_STAT
AC_FUNC_STRTOD
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([floor memmove memset modf pow sqrt strchr strdup strerror strrchr strstr strtol])

# make sure C++ 2011 extensions are enabled
AX_CXX_COMPILE_STDCXX_11()

# add a local directory as base for libs and include files
AC_MSG_CHECKING([for local include/lib path])

AC_ARG_WITH(local,[  --with-local[=dir]        search /usr/local or other directory for includes and libs],,)

if test "$with_local" != "no" -a "$with_local" != "" ; then
  
  if test "$with_local" = "yes" ; then
    ADD_PREFIX=/usr/local
  else
    ADD_PREFIX=$with_local
  fi

  AC_MSG_RESULT(adding $ADD_PREFIX)

  LOCAL_LIBS="-L$ADD_PREFIX/lib"
  LOCAL_INCS="-I$ADD_PREFIX/include"

  AC_SUBST(LOCAL_LIBS,$LOCAL_LIBS)
  AC_SUBST(LOCAL_INCS,$LOCAL_INCS)
else
  AC_MSG_RESULT([none])
fi

# conditionals

# wxwidgets
# the test could be extended to check for wx-config --version too
if test "`wx-config --list | grep unicode`" == "" ; then
  AC_MSG_ERROR(SAGA requires a unicode build of wxGTK)
  exit
fi

# postgresql/postgis
AX_LIB_POSTGRESQL([8.0])
if test "x$found_postgresql" = "xyes"; then
  echo "checking whether to enable postgresql... yes"
else
  echo "checking whether to enable postgresql... no"
fi
AM_CONDITIONAL(HAVE_POSTGRESQL, test "x$found_postgresql" = "xyes")

# debug
AC_ARG_ENABLE(debug,
     [  --enable-debug	  Turn on debugging],
     [case "${enableval}" in
        yes) debug=true ;;
        no)  debug=false ;;
        *) AC_MSG_ERROR(bad value ${enableval} for --enable-debug) ;;
      esac],[debug=false])
      AM_CONDITIONAL(DEBUG, test x$debug = xtrue)
 
# gui
AC_ARG_ENABLE(gui,
     [  --disable-gui		  Do not build GUI and interactive tools],
     [case "${enableval}" in
     	yes) gui=true ;;
	no) gui=false ;;
	*) AC_MSG_ERROR(bad value ${enableval} for --disable-gui) ;;
      esac],[gui=true])
      AM_CONDITIONAL(WITH_GUI, test x$gui = xtrue)

  if test $gui = "false" ; then
    AC_MSG_RESULT([        GUI and interactive tools will NOT be built])
  else
    AC_DEFINE([WITH_GUI], [1], ["built with GUI and interactive tools"])
    AC_MSG_RESULT([        GUI and interactive tools will be built])    
  fi

# odbc
AC_ARG_ENABLE(odbc,
     [  --disable-odbc		  Build without ODBC support],
     [case "${enableval}" in
     	yes) odbc=true ;;
	no) odbc=false ;;
	*) AC_MSG_ERROR(bad value ${enableval} for --disable-odbc) ;;
      esac],[odbc=true])
      AM_CONDITIONAL(WITH_ODBC, test x$odbc = xtrue)

  if test $odbc = "false" ; then
    AC_MSG_RESULT([        ODBC support will NOT be built])
  else
    AC_DEFINE([WITH_ODBC], [1], ["built with ODBC support"])
    AC_MSG_RESULT([        ODBC support will be built])    
  fi


# openmp
AC_ARG_ENABLE(openmp,
     [  --disable-openmp	  Build without OpenMP support],
     [case "${enableval}" in
        yes) openmp=true ;;
        no)  openmp=false ;;
        *) AC_MSG_ERROR(bad value ${enableval} for --disable-openmp) ;;
      esac],[openmp=true])
      AM_CONDITIONAL(OPENMP, test x$openmp = xtrue)
  
  if test $openmp = "false" ; then
    AC_MSG_RESULT([        OpenMP support disabled]) 
      GOMPFLAGS=""
  else
    if test -n "$GOMPFOUND" ; then 
      AC_MSG_RESULT([        Building with OpenMP support using libgomp])
      GOMPFLAGS="-fopenmp -lgomp"
    else 
      AC_MSG_RESULT([        libgomp not found, OpenMP support disabled]) 
      GOMPFLAGS=""
    fi
  fi

AC_SUBST(GOMPFLAGS)


# proj.4
PROJ_INC_DIR=
PROJ_LIB_DIR=

AC_ARG_WITH(
  [proj-includes],
  [AS_HELP_STRING([--with-proj-includes=DIR],[set custom path to PROJ includes)])],
  AC_CHECK_FILE([${withval}/proj_api.h], [PROJ_INC_DIR="$withval"], AC_MSG_ERROR([Custom PROJ include path not found]))
)

AC_ARG_WITH(
  [proj-libraries],
  [AS_HELP_STRING([--with-proj-libraries=DIR],[set custom path to PROJ libraries)])],
  AC_CHECK_FILE([${withval}/libproj.so], [PROJ_LIB_DIR="-L$withval"], AC_MSG_ERROR([Custom PROJ library path not found]))
)

if test -n "$PROJ_INC_DIR"; then
  AC_CHECK_FILE([${PROJ_INC_DIR}/projects.h], [PROJ4FOUND=1], [PROJ4FOUND=])
  AC_CHECK_FILE([${PROJ_INC_DIR}/projects.h], AC_CHECK_FILE([${PROJ_INC_DIR}/proj.h], [PROJ5FOUND=1], [PROJ5FOUND=]), [PROJ5FOUND=])
  AC_CHECK_FILE([${PROJ_INC_DIR}/proj.h], AC_CHECK_FILE([${PROJ_INC_DIR}/projects.h], [PROJ6FOUND=], [PROJ6FOUND=1]))
  PROJ_INC_DIR="-I$PROJ_INC_DIR"
fi

if test -n "$PROJ5FOUND" ; then 
  AC_MSG_RESULT([        using PROJ version 5])
  PROJ6FLAGS=""
else
  if test -n "$PROJ6FOUND" ; then 
    AC_MSG_RESULT([        using PROJ version 6]) 
    PROJ6FLAGS="-DPROJ6 -std=c++0x"
  else
    if test -n "$PROJ4FOUND" ; then 
      AC_MSG_RESULT([        using PROJ version 4]) 
      PROJ6FLAGS=""
    else
      AC_MSG_ERROR(SAGA requires the PROJ library)
      exit
    fi
  fi
fi

AC_SUBST(PROJ_INC_DIR)
AC_SUBST(PROJ_LIB_DIR)
AC_SUBST(PROJ6FLAGS)


# gdal
AX_LIB_GDAL([1.7])


AC_ARG_ENABLE(triangle,
        AS_HELP_STRING([--disable-triangle],[ Compile without triangle.c (only free for non-commercial usage), and use qhull instead (requires qhull)]),
	,
	[use_triangle=yes])

AM_CONDITIONAL(TRIANGLE, [test x$use_triangle = xyes])

QHULLFLAGS=""

if test "x$use_triangle" != xyes; then
    AC_MSG_RESULT([        Triangle disabled])
    AC_CHECK_HEADER([qhull/qhull_a.h],[AC_CHECK_LIB([qhull], [qh_qhull], QHULLFOUND=1,,)])
    AC_CHECK_HEADER([libqhull/qhull_a.h],[AC_CHECK_LIB([qhull], [qh_qhull], LIBQHULLFOUND=1,,)])
    if test -n "$LIBQHULLFOUND" ; then 
      AC_MSG_RESULT([        Building with libqhull])
      QHULLFLAGS="-DHAVE_LIBQHULL_QHULL_A_H"
    elif test -n "$QHULLFOUND" ; then 
      AC_MSG_RESULT([        Building with qhull])
      QHULLFLAGS="-DHAVE_QHULL_QHULL_A_H"
    else
      AC_MSG_FAILURE([--disable-triangle was given but test for qhull failed])
    fi
#    AC_CHECK_LIB(qhull, qh_qhull, , 
#      AC_MSG_FAILURE([--disable-triangle was given but test for qhull failed]))
#    AC_CHECK_HEADERS([qhull/qhull_a.h libqhull/qhull_a.h])
#    AC_SUBST([HAVE_QHULL_QHULL_A_H])
#    AC_SUBST([HAVE_LIBQHULL_QHULL_A_H])
 fi

AC_SUBST(QHULLFLAGS)

AC_ARG_ENABLE(libfire,
        AS_HELP_STRING([--disable-libfire],[ Compile without libfire (only free for non-commercial usage)]),
	,
	[use_libfire=yes])

AM_CONDITIONAL(LIBFIRE, [test x$use_libfire = xyes])

if test "x$use_libfire" != xyes; then
    AC_MSG_RESULT([        Libfire disabled])
fi


AC_ARG_WITH(system_dxflib,
    AS_HELP_STRING([--with-system-dxflib],[Use system version of dxflib instead of embedded copy, requires this library (and the header files)]),
    AC_MSG_RESULT([        using system libdxf]),
    system_libdxf=true,system_libdxf=false
    )
AM_CONDITIONAL(SYSTEM_DXFLIB, [test "$LIBDXFFOUND" -eq 1 -a x$system_libdxf = xtrue])

# check for --with-system-svm
AC_ARG_WITH([system_svm],
    AS_HELP_STRING([--with-system-svm],[Use system version of svm instead of embedded copy, requires this library (and the header files)]),
    system_svm=true,
    )

AC_MSG_CHECKING([usage of system libsvm])

if test "$with_system_svm" = yes ; then
    if test "$SVMFOUND" = 1 ; then
        AC_MSG_RESULT([yes])
        SYSTEM_SVM=yes
    else
        AC_MSG_FAILURE([--with-system-svm was given but test for libsvm failed])
    fi
else
    SYSTEM_SVM=no
    AC_MSG_RESULT([no])
fi

AM_CONDITIONAL([SYSTEM_SVM], [test x$SYSTEM_SVM = xyes])

#AM_CONDITIONAL(X86_64, test "`uname -m`" = "x86_64")
AM_CONDITIONAL(HAVE_CV, test -n "$CVFOUND")
AM_CONDITIONAL(HAVE_LLAS, test -n "$LASFOUND")
AM_CONDITIONAL(HAVE_PDAL, test -n "$PDALFOUND")
AM_CONDITIONAL(HAVE_VIGRA, test -n "$VIGRAFOUND" -a -n "$FFTWFOUND")
AM_CONDITIONAL(HAVE_HARU, test -n "$HARUFOUND")
AM_CONDITIONAL(HAVE_HDF5, test -n "$LIBHDF5FOUND")


if test -n "$CVFOUND" ; then 
  AC_MSG_RESULT([        Building with OpenCV])
fi

if test -n "$LASFOUND" ; then 
  AC_MSG_RESULT([        Building with libLAS])
fi

if test -n "$PDALFOUND" ; then 
  AC_MSG_RESULT([        Building with PDAL])
fi

if test -n "$VIGRAFOUND" -a -n "$FFTWFOUND" ; then 
  AC_MSG_RESULT([        Building with VIGRA])
fi

if test -n "$HARUFOUND" ; then 
  AC_MSG_RESULT([        using lhpdf (libharu) from system libs])
  DISABLELIBHARU=""
  HARU_LIB="-lhpdf"
else 
  AC_MSG_RESULT([        libharu was not found, pdf creation will be disabled]) 
  DISABLELIBHARU="-D_SAGA_DONOTUSE_HARU"
  HARU_LIB=""
fi

AC_SUBST(HARU_LIB)
AC_SUBST(DISABLELIBHARU)

AC_CHECK_DEFINE(HAVE_LIBCURL, AC_MSG_RESULT([        Building with libcurl]), AC_MSG_RESULT([        libcurl was not found: garden webservices will be build without https support]))


AC_ARG_ENABLE(python,
    [  --enable-python    Build python bindings for the saga api],
     [case "${enableval}" in
        yes) python=true ;;
        no)  python=false ;;
        *) AC_MSG_ERROR(bad value ${enableval} for --enable-python) ;;
      esac],[python=false])
      AM_CONDITIONAL(SAGA_PYTHON, test x$python = xtrue)
if test x$python = xtrue; then
AM_PATH_PYTHON
AX_PYTHON_DEVEL([])

AX_PKG_SWIG(1.3.17,[],[])
AX_SWIG_ENABLE_CXX
AX_SWIG_PYTHON
fi
#AC_CONFIG_FILES([Makefile])
AC_OUTPUT(Makefile \
	src/Makefile \
	src/saga_core/Makefile \
	src/saga_core/saga_api/Makefile \
	src/saga_core/saga_gdi/Makefile \
	src/saga_core/saga_gui/Makefile \
	src/saga_core/saga_gui/man/Makefile \
	src/saga_core/saga_cmd/Makefile \
	src/saga_core/saga_cmd/man/Makefile \
	src/tools/Makefile \
	src/tools/climate/Makefile \
	src/tools/climate/climate_tools/Makefile \
	src/tools/contrib/Makefile \
	src/tools/contrib/contrib_perego/Makefile \
	src/tools/db/Makefile \
	src/tools/db/db_odbc/Makefile \
	src/tools/db/db_pgsql/Makefile \
	src/tools/docs/Makefile \
	src/tools/docs/docs_html/Makefile \
	src/tools/docs/docs_pdf/Makefile \
	src/tools/garden/Makefile \
	src/tools/garden/garden_3d_viewer/Makefile \
	src/tools/garden/garden_fractals/Makefile \
	src/tools/garden/garden_games/Makefile \
	src/tools/garden/garden_learn_to_program/Makefile \
	src/tools/garden/garden_webservices/Makefile \
	src/tools/grid/Makefile \
	src/tools/grid/grid_analysis/Makefile \
	src/tools/grid/grid_calculus/Makefile \
	src/tools/grid/grid_calculus_bsl/Makefile \
	src/tools/grid/grid_filter/Makefile \
	src/tools/grid/grid_gridding/Makefile \
	src/tools/grid/grid_spline/Makefile \
	src/tools/grid/grid_tools/Makefile \
	src/tools/grid/grid_visualisation/Makefile \
	src/tools/grid/grids_tools/Makefile \
	src/tools/imagery/Makefile \
	src/tools/imagery/imagery_classification/Makefile \
	src/tools/imagery/imagery_opencv/Makefile \
	src/tools/imagery/imagery_maxent/Makefile \
	src/tools/imagery/imagery_photogrammetry/Makefile \
	src/tools/imagery/imagery_segmentation/Makefile \
	src/tools/imagery/imagery_svm/Makefile \
	src/tools/imagery/imagery_tools/Makefile \
	src/tools/imagery/imagery_vigra/Makefile \
	src/tools/io/Makefile \
	src/tools/io/io_esri_e00/Makefile \
	src/tools/io/io_gdal/Makefile \
	src/tools/io/io_gps/Makefile \
	src/tools/io/io_grid/Makefile \
	src/tools/io/io_grid_image/Makefile \
	src/tools/io/io_pdal/Makefile \
	src/tools/io/io_shapes/Makefile \
	src/tools/io/io_shapes_dxf/Makefile \
	src/tools/io/io_shapes_las/Makefile \
	src/tools/io/io_table/Makefile \
	src/tools/io/io_virtual/Makefile \
	src/tools/io/io_webservices/Makefile \
	src/tools/pointcloud/Makefile \
	src/tools/pointcloud/pointcloud_tools/Makefile \
	src/tools/pointcloud/pointcloud_viewer/Makefile \
	src/tools/projection/Makefile \
	src/tools/projection/pj_georeference/Makefile \
	src/tools/projection/pj_proj4/Makefile \
	src/tools/shapes/Makefile \
	src/tools/shapes/shapes_grid/Makefile \
	src/tools/shapes/shapes_lines/Makefile \
	src/tools/shapes/shapes_points/Makefile \
	src/tools/shapes/shapes_polygons/Makefile \
	src/tools/shapes/shapes_tools/Makefile \
	src/tools/shapes/shapes_transect/Makefile \
	src/tools/simulation/Makefile \
	src/tools/simulation/sim_air_flow/Makefile \
	src/tools/simulation/sim_cellular_automata/Makefile \
	src/tools/simulation/sim_ecosystems_hugget/Makefile \
	src/tools/simulation/sim_erosion/Makefile \
	src/tools/simulation/sim_fire_spreading/Makefile \
	src/tools/simulation/sim_geomorphology/Makefile \
	src/tools/simulation/sim_hydrology/Makefile \
	src/tools/simulation/sim_ihacres/Makefile \
	src/tools/simulation/sim_landscape_evolution/Makefile \
	src/tools/simulation/sim_qm_of_esp/Makefile \
	src/tools/simulation/sim_rivflow/Makefile \
	src/tools/statistics/Makefile \
	src/tools/statistics/statistics_grid/Makefile \
	src/tools/statistics/statistics_kriging/Makefile \
	src/tools/statistics/statistics_points/Makefile \
	src/tools/statistics/statistics_regression/Makefile \
	src/tools/table/Makefile \
	src/tools/table/table_calculus/Makefile \
	src/tools/table/table_tools/Makefile \
	src/tools/terrain_analysis/Makefile \
	src/tools/terrain_analysis/ta_channels/Makefile \
	src/tools/terrain_analysis/ta_cliffmetrics/Makefile \
	src/tools/terrain_analysis/ta_compound/Makefile \
	src/tools/terrain_analysis/ta_hydrology/Makefile \
	src/tools/terrain_analysis/ta_lighting/Makefile \
	src/tools/terrain_analysis/ta_morphometry/Makefile \
	src/tools/terrain_analysis/ta_preprocessor/Makefile \
	src/tools/terrain_analysis/ta_profiles/Makefile \
	src/tools/terrain_analysis/ta_slope_stability/Makefile \
	src/tools/tin/Makefile \
	src/tools/tin/tin_tools/Makefile \
	src/tools/tin/tin_viewer/Makefile \
)
