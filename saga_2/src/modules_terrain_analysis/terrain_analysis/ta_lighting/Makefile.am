DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS)
AM_LDFLAGS         = -shared $(DEP_LFLG)
pkglib_LTLIBRARIES = libta_lighting.la
libta_lighting_la_SOURCES =\
HillShade.cpp\
MLB_Interface.cpp\
SolarRadiation.cpp\
Visibility_Point.cpp\
HillShade.h\
MLB_Interface.h\
SolarRadiation.h\
Visibility_Point.h

libta_lighting_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

#install-exec-hook:
#	pushd .; cd $(pkglibdir); ln -s libta_lighting.so ta_lighting.so; popd
