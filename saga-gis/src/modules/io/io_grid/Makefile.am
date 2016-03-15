#
# $Id: Makefile.am 1277 2011-12-22 15:48:59Z reklov_w $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD -D_SAGA_UNICODE
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version
pkglib_LTLIBRARIES = libio_grid.la
libio_grid_la_SOURCES =\
bmp_export.cpp\
citygml_import.cpp\
erdas_lan.cpp\
esri_arcinfo.cpp\
grid_table.cpp\
import_clip_resample.cpp\
MLB_Interface.cpp\
mola.cpp\
raw.cpp\
srtm30.cpp\
surfer.cpp\
usgs_srtm.cpp\
wrf.cpp\
xyz.cpp\
bmp_export.h\
citygml_import.h\
erdas_lan.h\
esri_arcinfo.h\
grid_table.h\
import_clip_resample.h\
MLB_Interface.h\
mola.h\
raw.h\
srtm30.h\
surfer.h\
usgs_srtm.h\
wrf.h\
xyz.h

libio_grid_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

