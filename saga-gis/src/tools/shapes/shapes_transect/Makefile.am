#
# $Id$
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version -avoid-version
pkglib_LTLIBRARIES = libshapes_transect.la
libshapes_transect_la_SOURCES =\
Polygon_Transect.h\
MLB_Interface.h\
Polygon_Transect.cpp\
MLB_Interface.cpp

libshapes_transect_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

