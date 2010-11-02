#
# $Id: Makefile.am,v 1.12 2010-11-02 10:58:18 oconrad Exp $
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
pkglib_LTLIBRARIES = libgeostatistics_grid.la
libgeostatistics_grid_la_SOURCES =\
fast_representativeness.cpp\
grid_autocorrelation.cpp\
GSGrid_Directional_Statistics.cpp\
GSGrid_Residuals.cpp\
GSGrid_Statistics.cpp\
GSGrid_Variance.cpp\
GSGrid_Variance_Radius.cpp\
GSGrid_Zonal_Statistics.cpp\
MLB_Interface.cpp\
fast_representativeness.h\
grid_autocorrelation.h\
GSGrid_Directional_Statistics.h\
GSGrid_Residuals.h\
GSGrid_Statistics.h\
GSGrid_Variance.h\
GSGrid_Variance_Radius.h\
GSGrid_Zonal_Statistics.h\
MLB_Interface.h

libgeostatistics_grid_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

