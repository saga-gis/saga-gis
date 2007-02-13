#
# $Id: Makefile.am,v 1.4 2007-02-13 09:57:56 tschorr Exp $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS)
AM_LDFLAGS         = -fPIC -shared 
pkglib_LTLIBRARIES = libgeostatistics_grid.la
libgeostatistics_grid_la_SOURCES =\
GSGrid_Regression.cpp\
GSGrid_Regression_Multiple.cpp\
GSGrid_Residuals.cpp\
GSGrid_Statistics.cpp\
GSGrid_Variance.cpp\
GSGrid_Variance_Radius.cpp\
GSGrid_Zonal_Statistics.cpp\
MLB_Interface.cpp\
GSGrid_Regression.h\
GSGrid_Regression_Multiple.h\
GSGrid_Residuals.h\
GSGrid_Statistics.h\
GSGrid_Variance.h\
GSGrid_Variance_Radius.h\
GSGrid_Zonal_Statistics.h\
MLB_Interface.h

libgeostatistics_grid_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

