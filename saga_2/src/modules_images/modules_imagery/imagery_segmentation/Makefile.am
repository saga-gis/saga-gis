#
# $Id: Makefile.am,v 1.1 2009-12-04 15:57:20 oconrad Exp $
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
AM_LDFLAGS         = -fPIC -shared
pkglib_LTLIBRARIES = libimagery_segmentation.la
libimagery_segmentation_la_SOURCES =\
fast_region_growing.cpp\
fast_representativeness.cpp\
MLB_Interface.cpp\
fast_region_growing.h\
fast_representativeness.h\
MLB_Interface.h

libimagery_segmentation_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

