#
# $Id: Makefile.am 1277 2011-12-22 15:48:59Z reklov_w $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version
pkglib_LTLIBRARIES = libimagery_segmentation.la
libimagery_segmentation_la_SOURCES =\
connected_components.cpp\
grid_seeds.cpp\
MLB_Interface.cpp\
rga_basic.cpp\
skeletonization.cpp\
slic.cpp\
watershed_segmentation.cpp\
connected_components.h\
grid_seeds.h\
rga_basic.h\
skeletonization.h\
slic.h\
watershed_segmentation.h

libimagery_segmentation_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

