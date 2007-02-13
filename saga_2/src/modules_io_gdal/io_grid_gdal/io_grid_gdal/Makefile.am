#
# $Id: Makefile.am,v 1.4 2007-02-13 09:58:00 tschorr Exp $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
DEP_LIBS = -lgdal
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS)
AM_LDFLAGS         = -fPIC -shared $(DEP_LIBS)
pkglib_LTLIBRARIES = libio_grid_gdal.la
libio_grid_gdal_la_SOURCES =\
Grid_IO_GDAL.cpp\
MLB_Interface.cpp\
Grid_IO_GDAL.h\
MLB_Interface.h

libio_grid_gdal_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

