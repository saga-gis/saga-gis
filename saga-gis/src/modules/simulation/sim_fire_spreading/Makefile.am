#
# $Id: Makefile.am 1277 2011-12-22 15:48:59Z reklov_w $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD -D_SAGA_UNICODE
CXX_INCS           = -I$(top_srcdir)/src/saga_core -I$(top_srcdir)/src/modules/grid/grid_tools
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version
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

libsim_fire_spreading_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

