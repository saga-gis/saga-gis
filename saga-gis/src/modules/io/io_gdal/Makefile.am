#
# $Id: Makefile.am 1465 2012-08-23 09:20:00Z oconrad $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEP_LIBS = $(GDAL_LDFLAGS)
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD -D_SAGA_UNICODE
CXX_INCS           = -I$(top_srcdir)/src/saga_core $(GDAL_CFLAGS) $(GDAL2FLAGS)
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version $(DEP_LIBS)
pkglib_LTLIBRARIES = libio_gdal.la
libio_gdal_la_SOURCES =\
gdal_catalogue.cpp\
gdal_driver.cpp\
gdal_export.cpp\
gdal_export_geotiff.cpp\
gdal_import.cpp\
gdal_import_netcdf.cpp\
gdal_import_wms.cpp\
MLB_Interface.cpp\
ogr_driver.cpp\
ogr_export.cpp\
ogr_export_kml.cpp\
ogr_import.cpp\
gdal_catalogue.h\
gdal_driver.h\
gdal_export.h\
gdal_export_geotiff.h\
gdal_import.h\
gdal_import_netcdf.h\
gdal_import_wms.h\
MLB_Interface.h\
ogr_driver.h\
ogr_export.h\
ogr_export_kml.h\
ogr_import.h

libio_gdal_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

