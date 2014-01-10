#
# $Id: Makefile.am 1277 2011-12-22 15:48:59Z reklov_w $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
if SAGA_UNICODE
UC_DEFS = -D_SAGA_UNICODE
endif
if HAVE_LLAS
DEP_LIBS           = -llas
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(UC_DEFS) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version $(DEP_LIBS)
pkglib_LTLIBRARIES = libio_shapes_las.la
libio_shapes_las_la_SOURCES =\
las_export.cpp\
las_import.cpp\
las_info.cpp\
MLB_Interface.cpp\
las_export.h\
las_import.h\
las_info.h\
MLB_Interface.h

libio_shapes_las_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la $(ADD_MLBS)
endif
