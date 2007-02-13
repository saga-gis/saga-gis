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
pkglib_LTLIBRARIES = libgeostatistics_points.la
libgeostatistics_points_la_SOURCES =\
GSPoints_Semi_Variances.cpp\
MLB_Interface.cpp\
GSPoints_Semi_Variances.h\
MLB_Interface.h

libgeostatistics_points_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

