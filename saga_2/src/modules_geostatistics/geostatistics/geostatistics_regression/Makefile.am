#
# $Id: Makefile.am,v 1.1 2010-02-12 16:39:25 oconrad Exp $
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
pkglib_LTLIBRARIES = libgeostatistics_regression.la
libgeostatistics_regression_la_SOURCES =\
gw_regression.cpp\
MLB_Interface.cpp\
point_grid_regression.cpp\
point_multi_grid_regression.cpp\
point_trend_surface.cpp\
gw_regression.h\
MLB_Interface.h\
point_grid_regression.h\
point_multi_grid_regression.h\
point_trend_surface.h

libgeostatistics_regression_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

