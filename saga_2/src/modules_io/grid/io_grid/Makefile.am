DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS) `wx-config --cxxflags`
AM_LDFLAGS         = -shared $(DEP_LFLG)
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

#install-exec-hook:
#	pushd .; cd $(pkglibdir); ln -s libio_grid.so io_grid.so; popd
