#
# $Id: Makefile.am,v 1.7 2009-05-28 13:08:29 oconrad Exp $
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
pkglib_LTLIBRARIES = libshapes_lines.la
libshapes_lines_la_SOURCES =\
Lines_From_Points.cpp\
Lines_From_Polygons.cpp\
line_properties.cpp\
MLB_Interface.cpp\
Lines_From_Points.h\
Lines_From_Polygons.h\
line_properties.h\
MLB_Interface.h

libshapes_lines_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

