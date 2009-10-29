#
# $Id: Makefile.am,v 1.4 2009-10-29 20:22:52 reklov_w Exp $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
if SAGA_UNICODE
UC_DEFS = -D_SAGA_UNICODE
endif
DEP_LIBS           = -llas
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(UC_DEFS) $(DBGFLAGS)
AM_LDFLAGS         = -fPIC -shared $(DEP_LIBS)
pkglib_LTLIBRARIES = libio_shapes_las.la
libio_shapes_las_la_SOURCES =\
las_import.cpp\
MLB_Interface.cpp\
las_import.h\
MLB_Interface.h

libio_shapes_las_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la $(ADD_MLBS)

