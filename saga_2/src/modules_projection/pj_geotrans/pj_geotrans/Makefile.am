DEP_LIBS = -lgeotrans

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS) `wx-config --cxxflags`
AM_LDFLAGS         = -shared $(DEP_LFLG) $(DEP_LIBS)
pkglib_LTLIBRARIES = libpj_geotrans.la
libpj_geotrans_la_SOURCES =\
GEOTRANS_Base.cpp\
GEOTRANS_Grid.cpp\
GEOTRANS_Shapes.cpp\
MLB_Interface.cpp\
GEOTRANS_Base.h\
GEOTRANS_Grid.h\
GEOTRANS_Shapes.h\
MLB_Interface.h

libpj_geotrans_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

#install-exec-hook:
#	pushd .; cd $(pkglibdir); ln -s libpj_geotrans.so pj_geotrans.so; popd
