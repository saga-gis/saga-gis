ADD_MLBS = $(top_srcdir)/src/modules/grid/grid_tools/libgrid_tools.la

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS) `wx-config --cxxflags`
AM_LDFLAGS         = -shared $(DEP_LFLG)
pkglib_LTLIBRARIES = libsim_fire_spreading.la
libsim_fire_spreading_la_SOURCES =\
fireLib.c\
Forecasting.cpp\
MLB_Interface.cpp\
Simulate.cpp\
fireLib.h\
Forecasting.h\
MLB_Interface.h\
Simulate.h

libsim_fire_spreading_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la $(ADD_MLBS)

#install-exec-hook:
#	pushd .; cd $(pkglibdir); ln -s libsim_fire_spreading.so sim_fire_spreading.so; popd
