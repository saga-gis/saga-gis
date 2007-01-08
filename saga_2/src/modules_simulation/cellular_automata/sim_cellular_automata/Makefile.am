DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS) `wx-config --cxxflags`
AM_LDFLAGS         = -shared $(DEP_LFLG)
pkglib_LTLIBRARIES = libsim_cellular_automata.la
libsim_cellular_automata_la_SOURCES =\
Life.cpp\
MLB_Interface.cpp\
Wator.cpp\
Life.h\
MLB_Interface.h\
Wator.h

libsim_cellular_automata_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

#install-exec-hook:
#	pushd .; cd $(pkglibdir); ln -s libsim_cellular_automata.so sim_cellular_automata.so; popd
