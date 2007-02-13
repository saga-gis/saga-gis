#
# $Id: Makefile.am,v 1.5 2007-02-13 09:58:04 tschorr Exp $
#
ADD_MLBS = $(top_srcdir)/src/modules/grid/grid_tools/libgrid_tools.la
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS)
AM_LDFLAGS         = -fPIC -shared
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

