DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS)
AM_LDFLAGS         = -shared $(DEP_LFLG) $(DEP_LIBS)
pkglib_LTLIBRARIES = libio_shapes.la
libio_shapes_la_SOURCES =\
atlas_bna.cpp\
generate.cpp\
gstat.cpp\
MLB_Interface.cpp\
surfer_bln.cpp\
xyz.cpp\
atlas_bna.h\
generate.h\
gstat.h\
MLB_Interface.h\
surfer_bln.h\
xyz.h

libio_shapes_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la $(ADD_MLBS)

#install-exec-hook:
#	pushd .; cd $(pkglibdir); ln -s libio_shapes.so io_shapes.so; popd
