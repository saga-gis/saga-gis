#
# $Id$
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version
pkglib_LTLIBRARIES = libgrid_gridding.la
libgrid_gridding_la_SOURCES =\
grid_cell_polygon_coverage.cpp\
Interpolation.cpp\
Interpolation_AngularDistance.cpp\
Interpolation_InverseDistance.cpp\
Interpolation_NaturalNeighbour.cpp\
Interpolation_NearestNeighbour.cpp\
Interpolation_Shepard.cpp\
Interpolation_Triangulation.cpp\
kernel_density.cpp\
MLB_Interface.cpp\
Shapes2Grid.cpp\
Shepard.cpp\
./nn/delaunay.c\
./nn/hash.c\
./nn/istack.c\
./nn/lpi.c\
./nn/nnai.c\
./nn/nncommon.c\
./nn/nncommon-vulnerable.c\
./nn/nnpi.c\
grid_cell_polygon_coverage.h\
Interpolation.h\
Interpolation_AngularDistance.h\
Interpolation_InverseDistance.h\
Interpolation_NaturalNeighbour.h\
Interpolation_NearestNeighbour.h\
Interpolation_Shepard.h\
Interpolation_Triangulation.h\
kernel_density.h\
MLB_Interface.h\
Shapes2Grid.h\
Shepard.h\
./nn/config.h\
./nn/delaunay.h\
./nn/hash.h\
./nn/istack.h\
./nn/nan.h\
./nn/nn.h\
./nn/nn_internal.h\
./nn/version.h

if TRIANGLE
libgrid_gridding_la_SOURCES +=\
./nn/triangle.c\
./nn/triangle.h
else
AM_CFLAGS          = -DUSE_QHULL $(QHULLFLAGS)
if QHULL_R
AM_LDFLAGS         += -lqhull_r
else
AM_LDFLAGS         += -lqhull
endif
endif

libgrid_gridding_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

