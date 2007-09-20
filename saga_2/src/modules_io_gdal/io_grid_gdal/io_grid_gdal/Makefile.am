#
# $Id: Makefile.am,v 1.7 2007-09-20 14:41:39 oconrad Exp $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
if SAGA_UNICODE
UC_DEFS = -D_SAGA_UNICODE
endif
DEP_LIBS = `gdal-config --libs`
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core `gdal-config --cflags`
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(UC_DEFS) $(DBGFLAGS)
AM_LDFLAGS         = -fPIC -shared $(DEP_LIBS)
pkglib_LTLIBRARIES = libio_grid_gdal.la
libio_grid_gdal_la_SOURCES =\
gdal_driver.cpp\
gdal_export.cpp\
gdal_export_geotiff.cpp\
gdal_import.cpp\
MLB_Interface.cpp\
gdal_driver.h\
gdal_export.h\
gdal_export_geotiff.h\
gdal_import.h\
MLB_Interface.h

libio_grid_gdal_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

