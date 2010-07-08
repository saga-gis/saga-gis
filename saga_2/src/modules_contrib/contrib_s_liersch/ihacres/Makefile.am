#
# $Id: Makefile.am,v 1.2 2010-07-08 14:11:48 johanvdw Exp $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
if SAGA_UNICODE
UC_DEFS = -D_SAGA_UNICODE
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(UC_DEFS) $(DBGFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version
pkglib_LTLIBRARIES = libihacres.la
libihacres_la_SOURCES =\
convert_sl.cpp\
ihacres_basin.cpp\
ihacres_cal2.cpp\
ihacres_elev.cpp\
ihacres_elev_bands.cpp\
ihacres_elev_cal.cpp\
ihacres_eq.cpp\
ihacres_v1.cpp\
MLB_Interface.cpp\
model_tools.cpp\
snow_module.cpp\
convert_sl.h\
ihacres_basin.h\
ihacres_cal2.h\
ihacres_elev.h\
ihacres_elev_bands.h\
ihacres_elev_cal.h\
ihacres_eq.h\
ihacres_v1.h\
MLB_Interface.h\
model_tools.h\
snow_module.h

libihacres_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

