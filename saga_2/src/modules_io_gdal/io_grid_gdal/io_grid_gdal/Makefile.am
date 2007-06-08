DEP_LIBS = -lgdal

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS)
AM_LDFLAGS         = -shared $(DEP_LFLG) $(DEP_LIBS)
pkglib_LTLIBRARIES = libio_grid_gdal.la
libio_grid_gdal_la_SOURCES =\
Grid_IO_GDAL.cpp\
MLB_Interface.cpp\
Grid_IO_GDAL.h\
MLB_Interface.h

libio_grid_gdal_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

#install-exec-hook:
#	pushd .; cd $(pkglibdir); ln -s libio_grid_gdal.so io_grid_gdal.so; popd
