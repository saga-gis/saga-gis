#
# $Id: Makefile.am,v 1.3 2010-07-08 14:11:48 johanvdw Exp $
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
pkglib_LTLIBRARIES = libgeostatistics_regression.la
libgeostatistics_regression_la_SOURCES =\
gw_multi_regression.cpp\
gw_multi_regression_grid.cpp\
gw_regression.cpp\
gw_regression_grid.cpp\
MLB_Interface.cpp\
point_grid_regression.cpp\
point_multi_grid_regression.cpp\
point_trend_surface.cpp\
gw_multi_regression.h\
gw_multi_regression_grid.h\
gw_regression.h\
gw_regression_grid.h\
MLB_Interface.h\
point_grid_regression.h\
point_multi_grid_regression.h\
point_trend_surface.h

libgeostatistics_regression_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

