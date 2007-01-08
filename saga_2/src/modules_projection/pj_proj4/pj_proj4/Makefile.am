DEP_LIBS = -lproj

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS) `wx-config --cxxflags`
AM_LDFLAGS         = -shared $(DEP_LFLG) $(DEP_LIBS)
pkglib_LTLIBRARIES = libpj_proj4.la
libpj_proj4_la_SOURCES =\
MLB_Interface.cpp\
PROJ4_Base.cpp\
PROJ4_Grid.cpp\
PROJ4_Shapes.cpp\
MLB_Interface.h\
PROJ4_Base.h\
PROJ4_Grid.h\
PROJ4_Shapes.h

libpj_proj4_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

#install-exec-hook:
#	pushd .; cd $(pkglibdir); ln -s libpj_proj4.so pj_proj4.so; popd
