#
# $Id: Makefile.am 1738 2013-06-20 11:15:45Z reklov_w $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version 
pkglib_LTLIBRARIES = libstatistics_regression.la
libstatistics_regression_la_SOURCES =\
grid_multi_grid_regression.cpp\
gw_multi_regression.cpp\
gw_multi_regression_grid.cpp\
gw_multi_regression_points.cpp\
gw_regression.cpp\
gw_regression_grid.cpp\
gwr_grid_downscaling.cpp\
grids_trend_polynom.cpp\
MLB_Interface.cpp\
point_grid_regression.cpp\
point_multi_grid_regression.cpp\
point_trend_surface.cpp\
point_zonal_multi_grid_regression.cpp\
table_regression_multiple.cpp\
table_trend.cpp\
grid_multi_grid_regression.h\
gw_multi_regression.h\
gw_multi_regression_grid.h\
gw_multi_regression_points.h\
gw_regression.h\
gw_regression_grid.h\
gwr_grid_downscaling.h\
grids_trend_polynom.h\
MLB_Interface.h\
point_grid_regression.h\
point_multi_grid_regression.h\
point_trend_surface.h\
point_zonal_multi_grid_regression.h\
table_regression_multiple.h\
table_trend.h

libstatistics_regression_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

