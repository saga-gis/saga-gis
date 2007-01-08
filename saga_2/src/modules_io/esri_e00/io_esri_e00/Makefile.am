DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS) `wx-config --cxxflags`
AM_LDFLAGS         = -shared $(DEP_LFLG)
pkglib_LTLIBRARIES = libio_esri_e00.la
libio_esri_e00_la_SOURCES =\
ESRI_E00_Import.cpp\
MLB_Interface.cpp\
e00compr/cpl_conv.c\
e00compr/cpl_error.c\
e00compr/cpl_vsisimple.c\
e00compr/e00read.c\
e00compr/e00write.c\
ESRI_E00_Import.h\
MLB_Interface.h\
e00compr/cpl_conv.h\
e00compr/cpl_error.h\
e00compr/cpl_port.h\
e00compr/cpl_vsi.h\
e00compr/e00compr.h

libio_esri_e00_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

#install-exec-hook:
#	pushd .; cd $(pkglibdir); ln -s libio_esri_e00.so io_esri_e00.so; popd
