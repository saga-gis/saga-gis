DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS)
AM_LDFLAGS         = -shared $(DEP_LFLG)
pkglib_LTLIBRARIES = libgrid_gridding.la
libgrid_gridding_la_SOURCES =\
Interpolation.cpp\
Interpolation_InverseDistance.cpp\
Interpolation_NearestNeighbour.cpp\
Interpolation_Shepard.cpp\
Interpolation_Triangulation.cpp\
MLB_Interface.cpp\
Shapes2Grid.cpp\
Shepard.cpp\
Interpolation.h\
Interpolation_InverseDistance.h\
Interpolation_NearestNeighbour.h\
Interpolation_Shepard.h\
Interpolation_Triangulation.h\
MLB_Interface.h\
Shapes2Grid.h\
Shepard.h

libgrid_gridding_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

#install-exec-hook:
#	pushd .; cd $(pkglibdir); ln -s libgrid_gridding.so grid_gridding.so; popd
