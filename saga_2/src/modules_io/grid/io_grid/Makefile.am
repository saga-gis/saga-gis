#
# $Id: Makefile.am,v 1.5 2007-02-13 09:57:56 tschorr Exp $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS) $(DBGFLAGS)
AM_LDFLAGS         = -fPIC -shared $(DEP_LFLG)
pkglib_LTLIBRARIES = libio_grid.la
libio_grid_la_SOURCES =\
bmp_export.cpp\
erdas_lan.cpp\
esri_arcinfo.cpp\
grid_table.cpp\
MLB_Interface.cpp\
mola.cpp\
raw.cpp\
srtm30.cpp\
surfer.cpp\
usgs_srtm.cpp\
xyz.cpp\
bmp_export.h\
erdas_lan.h\
esri_arcinfo.h\
grid_table.h\
MLB_Interface.h\
mola.h\
raw.h\
srtm30.h\
surfer.h\
usgs_srtm.h\
xyz.h

libio_grid_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

