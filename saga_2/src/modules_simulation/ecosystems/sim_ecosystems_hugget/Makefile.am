DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS)
AM_LDFLAGS         = -shared $(DEP_LFLG)
pkglib_LTLIBRARIES = libsim_ecosystems_hugget.la
libsim_ecosystems_hugget_la_SOURCES =\
Hugget_01.cpp\
Hugget_02.cpp\
Hugget_03.cpp\
MLB_Interface.cpp\
Hugget_01.h\
Hugget_02.h\
Hugget_03.h\
MLB_Interface.h

libsim_ecosystems_hugget_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

#install-exec-hook:
#	pushd .; cd $(pkglibdir); ln -s libsim_ecosystems_hugget.so sim_ecosystems_hugget.so; popd
