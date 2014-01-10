#
# $Id: Makefile.am 1277 2011-12-22 15:48:59Z reklov_w $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
if SAGA_UNICODE
UC_DEFS = -D_SAGA_UNICODE
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(UC_DEFS) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version
pkglib_LTLIBRARIES = libstatistics_points.la
libstatistics_points_la_SOURCES =\
GSPoints_Distances.cpp\
GSPoints_Pattern_Analysis.cpp\
GSPoints_Semi_Variances.cpp\
GSPoints_Variogram_Cloud.cpp\
GSPoints_Variogram_Surface.cpp\
MLB_Interface.cpp\
GSPoints_Distances.h\
GSPoints_Pattern_Analysis.h\
GSPoints_Semi_Variances.h\
GSPoints_Variogram_Cloud.h\
GSPoints_Variogram_Surface.h\
MLB_Interface.h

libstatistics_points_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

