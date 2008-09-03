#
# $Id: Makefile.am,v 1.1 2008-09-03 13:27:09 oconrad Exp $
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
pkglib_LTLIBRARIES = libio_gdal.la
libio_gdal_la_SOURCES =\
gdal_driver.cpp\
gdal_export.cpp\
gdal_export_geotiff.cpp\
gdal_import.cpp\
MLB_Interface.cpp\
ogr_driver.cpp\
ogr_export.cpp\
ogr_import.cpp\
gdal_driver.h\
gdal_export.h\
gdal_export_geotiff.h\
gdal_import.h\
MLB_Interface.h\
ogr_driver.h\
ogr_export.h\
ogr_import.h

libio_gdal_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

