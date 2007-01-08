DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS) `wx-config --cxxflags`
AM_LDFLAGS         = -shared $(DEP_LFLG)
pkglib_LTLIBRARIES = libta_preprocessor.la
libta_preprocessor_la_SOURCES =\
FillSinks.cpp\
MLB_Interface.cpp\
Pit_Eliminator.cpp\
Pit_Router.cpp\
FillSinks.h\
MLB_Interface.h\
Pit_Eliminator.h\
Pit_Router.h

libta_preprocessor_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

#install-exec-hook:
#	pushd .; cd $(pkglibdir); ln -s libta_preprocessor.so ta_preprocessor.so; popd
