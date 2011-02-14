#
# $Id$
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
if SAGA_UNICODE
DEP_DEFS = -D_SAGA_UNICODE
endif
if SAGA_UNICODE
UC_DEFS = -D_SAGA_UNICODE
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEP_DEFS) $(DEF_SAGA) $(UC_DEFS) $(DBGFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version
pkglib_LTLIBRARIES = libgrid_gridding.la
libgrid_gridding_la_SOURCES =\
Interpolation.cpp\
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
./nn/nnpi.c\
./nn/triangle.c\
Interpolation.h\
Interpolation_InverseDistance.h\
Interpolation_NaturalNeighbour.h\
Interpolation_NearestNeighbour.h\
Interpolation_Shepard.h\
Interpolation_Triangulation.h\
kernel_density.h\
MLB_Interface.h\
Shapes2Grid.h\
Shepard.h\
./nn/delaunay.h\
./nn/hash.h\
./nn/istack.h\
./nn/nan.h\
./nn/nn.h\
./nn/triangle.h\
./nn/version.h

libgrid_gridding_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

