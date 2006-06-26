DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS)
AM_LDFLAGS         = -shared $(DEP_LFLG)
pkglib_LTLIBRARIES = libio_shapes.la
libio_shapes_la_SOURCES =\
generate.cpp\
gstat.cpp\
MLB_Interface.cpp\
surfer_bln.cpp\
xyz.cpp\
Generate.h\
GSTAT.H\
MLB_Interface.h\
surfer_bln.h\
XYZ.H

libio_shapes_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

#install-exec-hook:
#	pushd .; cd $(pkglibdir); ln -s libio_shapes.so io_shapes.so; popd
