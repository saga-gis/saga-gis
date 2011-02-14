#
# $Id: Makefile.am 911 2011-02-14 16:38:15Z reklov_w $
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
pkglib_LTLIBRARIES = libshapes_lines.la
libshapes_lines_la_SOURCES =\
line_dissolve.cpp\
Lines_From_Points.cpp\
Lines_From_Polygons.cpp\
line_polygon_intersection.cpp\
line_properties.cpp\
line_simplification.cpp\
MLB_Interface.cpp\
line_dissolve.h\
Lines_From_Points.h\
Lines_From_Polygons.h\
line_polygon_intersection.h\
line_properties.h\
line_simplification.h\
MLB_Interface.h

libshapes_lines_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

