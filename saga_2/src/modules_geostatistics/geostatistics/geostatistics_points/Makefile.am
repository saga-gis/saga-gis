#
# $Id: Makefile.am,v 1.7 2010-02-22 16:21:57 oconrad Exp $
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
pkglib_LTLIBRARIES = libgeostatistics_points.la
libgeostatistics_points_la_SOURCES =\
GSPoints_Distances.cpp\
GSPoints_Semi_Variances.cpp\
GSPoints_Variogram_Cloud.cpp\
GSPoints_Variogram_Surface.cpp\
MLB_Interface.cpp\
GSPoints_Distances.h\
GSPoints_Semi_Variances.h\
GSPoints_Variogram_Cloud.h\
GSPoints_Variogram_Surface.h\
MLB_Interface.h

libgeostatistics_points_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

