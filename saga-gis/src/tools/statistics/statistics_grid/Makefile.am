#
# $Id: Makefile.am 1554 2012-12-05 18:29:04Z oconrad $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD -D_SAGA_UNICODE
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version 
pkglib_LTLIBRARIES = libstatistics_grid.la
libstatistics_grid_la_SOURCES =\
categorical_variation.cpp\
fast_representativeness.cpp\
grid_autocorrelation.cpp\
grid_latlon_statistics.cpp\
grid_pca.cpp\
grid_pca_focal.cpp\
grid_statistics_from_files.cpp\
grid_statistics_successive.cpp\
multiband_variation.cpp\
GSGrid_Directional_Statistics.cpp\
GSGrid_Residuals.cpp\
GSGrid_Statistics.cpp\
GSGrid_Variance.cpp\
GSGrid_Variance_Radius.cpp\
GSGrid_Zonal_Statistics.cpp\
MLB_Interface.cpp\
categorical_variation.h\
fast_representativeness.h\
grid_autocorrelation.h\
grid_latlon_statistics.h\
grid_pca.h\
grid_pca_focal.h\
grid_statistics_from_files.h\
grid_statistics_successive.h\
multiband_variation.h\
GSGrid_Directional_Statistics.h\
GSGrid_Residuals.h\
GSGrid_Statistics.h\
GSGrid_Variance.h\
GSGrid_Variance_Radius.h\
GSGrid_Zonal_Statistics.h

libstatistics_grid_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

