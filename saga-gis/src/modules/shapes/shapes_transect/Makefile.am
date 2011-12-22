#
# $Id$
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
AM_LDFLAGS         = -fPIC -shared -avoid-version -avoid-version
pkglib_LTLIBRARIES = libtransect.la
libtransect_la_SOURCES =\
Polygon_Transect.h\
MLB_Interface.h\
Polygon_Transect.cpp\
MLB_Interface.cpp

libtransect_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

