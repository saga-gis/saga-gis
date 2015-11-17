# $Id$
#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(saga, 2.2.3, BUG-REPORT-ADDRESS)
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
AC_CHECK_LIB([cv], [cvGetRows], CVFOUND=1,,)
AC_CHECK_LIB([geotrans], [Set_Mercator_Coordinates])
AC_CHECK_LIB([fftw3],[fftw_execute],FFTWFOUND=1,)
AC_CHECK_LIB([jasper], [jas_calloc],JASPERFOUND=1,,)
AC_CHECK_LIB([las], [LAS_GetVersion], LASFOUND=1,,)
AC_CHECK_LIB(hpdf, HPDF_Pages_New, HARUFOUND=1,,)
AC_CHECK_LIB([odbc], [SQLFetch], ODBCFOUND=1,[ODBC \(libodbc\) library not found])
AC_CHECK_LIB([proj], [pj_is_latlong], PROJFOUND=1,,)
AC_CHECK_LIB([vigraimpex], [VIGRA_RGBE_ReadPixels_Raw],VIGRAFOUND=1,)
AC_CHECK_LIB([gomp], [omp_get_num_threads], GOMPFOUND=1,,)

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
if test "`wx-config --list | grep unicode`" != "" ; then
  AM_CONDITIONAL(SAGA_UNICODE, true)
else
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
     [  --disable-gui		  Do not build GUI and interactive modules],
     [case "${enableval}" in
     	yes) gui=true ;;
	no) gui=false ;;
	*) AC_MSG_ERROR(bad value ${enableval} for --disable-gui) ;;
      esac],[gui=true])
      AM_CONDITIONAL(WITH_GUI, test x$gui = xtrue)

  if test $gui = "false" ; then
    AC_MSG_RESULT([        GUI and interactive modules will NOT be built])
  else
    AC_DEFINE([WITH_GUI], [1], ["built with GUI and interactive modules"])
    AC_MSG_RESULT([        GUI and interactive modules will be built])    
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

#AM_CONDITIONAL(X86_64, test "`uname -m`" = "x86_64")
    AM_CONDITIONAL(HAVE_CV, test -n "$CVFOUND")
    AM_CONDITIONAL(HAVE_LLAS, test -n "$LASFOUND")
    AM_CONDITIONAL(HAVE_VIGRA, test -n "$VIGRAFOUND" -a -n "$FFTWFOUND")
    AM_CONDITIONAL(HAVE_HARU, test -n "$HARUFOUND")

  if test -n "$HARUFOUND" ; then 
    AC_MSG_RESULT([        using lhpdf (libharu) from system libs.])
    DISABLELIBHARU=""
    HARU_LIB="-lhpdf"
  else 
    AC_MSG_RESULT([        libharu was not found, pdf creation will be disabled]) 
    DISABLELIBHARU="-D_SAGA_DONOTUSE_HARU"
    HARU_LIB=""
  fi

AC_SUBST(HARU_LIB)
AC_SUBST(DISABLELIBHARU)

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
	src/modules/Makefile \
	src/modules/climate/Makefile \
	src/modules/climate/climate_tools/Makefile \
	src/modules/contrib/Makefile \
	src/modules/contrib/contrib_peregro/Makefile \
	src/modules/db/Makefile \
	src/modules/db/db_odbc/Makefile \
	src/modules/db/db_pgsql/Makefile \
	src/modules/docs/Makefile \
	src/modules/docs/docs_html/Makefile \
	src/modules/docs/docs_pdf/Makefile \
	src/modules/garden/Makefile \
	src/modules/garden/garden_3d_viewer/Makefile \
	src/modules/garden/garden_fractals/Makefile \
	src/modules/garden/garden_games/Makefile \
	src/modules/garden/garden_learn_to_program/Makefile \
	src/modules/garden/garden_webservices/Makefile \
	src/modules/grid/Makefile \
	src/modules/grid/grid_analysis/Makefile \
	src/modules/grid/grid_calculus/Makefile \
	src/modules/grid/grid_calculus_bsl/Makefile \
	src/modules/grid/grid_filter/Makefile \
	src/modules/grid/grid_gridding/Makefile \
	src/modules/grid/grid_spline/Makefile \
	src/modules/grid/grid_tools/Makefile \
	src/modules/grid/grid_visualisation/Makefile \
	src/modules/imagery/Makefile \
	src/modules/imagery/imagery_classification/Makefile \
	src/modules/imagery/imagery_opencv/Makefile \
	src/modules/imagery/imagery_maxent/Makefile \
	src/modules/imagery/imagery_photogrammetry/Makefile \
	src/modules/imagery/imagery_segmentation/Makefile \
	src/modules/imagery/imagery_svm/Makefile \
	src/modules/imagery/imagery_tools/Makefile \
	src/modules/imagery/imagery_vigra/Makefile \
	src/modules/io/Makefile \
	src/modules/io/io_esri_e00/Makefile \
	src/modules/io/io_gdal/Makefile \
	src/modules/io/io_gps/Makefile \
	src/modules/io/io_grid/Makefile \
	src/modules/io/io_grid_grib2/Makefile \
	src/modules/io/io_grid_image/Makefile \
	src/modules/io/io_shapes/Makefile \
	src/modules/io/io_shapes_dxf/Makefile \
	src/modules/io/io_shapes_las/Makefile \
	src/modules/io/io_table/Makefile \
	src/modules/io/io_virtual/Makefile \
	src/modules/pointcloud/Makefile \
	src/modules/pointcloud/pointcloud_tools/Makefile \
	src/modules/pointcloud/pointcloud_viewer/Makefile \
	src/modules/projection/Makefile \
	src/modules/projection/pj_georeference/Makefile \
	src/modules/projection/pj_proj4/Makefile \
	src/modules/shapes/Makefile \
	src/modules/shapes/shapes_grid/Makefile \
	src/modules/shapes/shapes_lines/Makefile \
	src/modules/shapes/shapes_points/Makefile \
	src/modules/shapes/shapes_polygons/Makefile \
	src/modules/shapes/shapes_tools/Makefile \
	src/modules/shapes/shapes_transect/Makefile \
	src/modules/simulation/Makefile \
	src/modules/simulation/sim_cellular_automata/Makefile \
	src/modules/simulation/sim_ecosystems_hugget/Makefile \
	src/modules/simulation/sim_erosion/Makefile \
	src/modules/simulation/sim_fire_spreading/Makefile \
	src/modules/simulation/sim_hydrology/Makefile \
	src/modules/simulation/sim_ihacres/Makefile \
	src/modules/simulation/sim_qm_of_esp/Makefile \
	src/modules/simulation/sim_rivflow/Makefile \
	src/modules/statistics/Makefile \
	src/modules/statistics/statistics_grid/Makefile \
	src/modules/statistics/statistics_kriging/Makefile \
	src/modules/statistics/statistics_points/Makefile \
	src/modules/statistics/statistics_regression/Makefile \
	src/modules/table/Makefile \
	src/modules/table/table_calculus/Makefile \
	src/modules/table/table_tools/Makefile \
	src/modules/terrain_analysis/Makefile \
	src/modules/terrain_analysis/ta_channels/Makefile \
	src/modules/terrain_analysis/ta_compound/Makefile \
	src/modules/terrain_analysis/ta_hydrology/Makefile \
	src/modules/terrain_analysis/ta_lighting/Makefile \
	src/modules/terrain_analysis/ta_morphometry/Makefile \
	src/modules/terrain_analysis/ta_preprocessor/Makefile \
	src/modules/terrain_analysis/ta_profiles/Makefile \
	src/modules/terrain_analysis/ta_slope_stability/Makefile \
	src/modules/tin/Makefile \
	src/modules/tin/tin_tools/Makefile \
	src/modules/tin/tin_viewer/Makefile \
)
