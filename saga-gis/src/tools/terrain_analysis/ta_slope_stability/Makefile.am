#
# $Id: Makefile.am 1277 2011-12-22 15:48:59Z reklov_w $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version 
pkglib_LTLIBRARIES = libta_slope_stability.la
libta_slope_stability_la_SOURCES =\
ANGMAP_01.cpp\
MLB_Interface.cpp\
Safetyfactor_01.cpp\
SHALSTAB_01.cpp\
TOBIA_01.cpp\
WEDGEFAIL_01.cpp\
WETNESS_01.cpp\
ANGMAP_01.h\
MLB_Interface.h\
Safetyfactor_01.h\
SHALSTAB_01.h\
TOBIA_01.h\
WEDGEFAIL_01.h\
WETNESS_01.h

libta_slope_stability_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

