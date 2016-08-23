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
pkglib_LTLIBRARIES = libsim_ihacres.la
libsim_ihacres_la_SOURCES =\
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

libsim_ihacres_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

