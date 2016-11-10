#
# $Id: Makefile.am 1277 2011-12-22 15:48:59Z reklov_w $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD -D_SAGA_UNICODE
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version 
pkglib_LTLIBRARIES = libclimate_tools.la
libclimate_tools_la_SOURCES =\
bioclimatic_vars.cpp\
climate_tools.cpp\
daily_sun.cpp\
etp_hargreave.cpp\
frost_change_frequency.cpp\
grid_levels_interpolation.cpp\
milankovic.cpp\
MLB_Interface.cpp\
thermal_belts.cpp\
treeline.cpp\
windeffect_correction.cpp\
bioclimatic_vars.h\
climate_tools.h\
daily_sun.h\
etp_hargreave.h\
frost_change_frequency.h\
grid_levels_interpolation.h\
milankovic.h\
MLB_Interface.h\
thermal_belts.h\
treeline.h\
windeffect_correction.h

libclimate_tools_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

