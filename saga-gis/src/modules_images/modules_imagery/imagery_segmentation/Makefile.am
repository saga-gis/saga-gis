#
# $Id: Makefile.am,v 1.5 2011-01-24 14:50:49 oconrad Exp $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
if SAGA_UNICODE
UC_DEFS = -D_SAGA_UNICODE
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(UC_DEFS) $(DBGFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version
pkglib_LTLIBRARIES = libimagery_segmentation.la
libimagery_segmentation_la_SOURCES =\
grid_seeds.cpp\
MLB_Interface.cpp\
rga_basic.cpp\
skeletonization.cpp\
watershed_segmentation.cpp\
grid_seeds.h\
MLB_Interface.h\
rga_basic.h\
skeletonization.h\
watershed_segmentation.h

libimagery_segmentation_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

