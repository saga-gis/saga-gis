#
# $Id: Makefile.am 1277 2011-12-22 15:48:59Z reklov_w $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
if SAGA_UNICODE
UC_DEFS = -D_SAGA_UNICODE
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(UC_DEFS) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version 
pkglib_LTLIBRARIES = libio_gps.la
libio_gps_la_SOURCES =\
GPSBabel.cpp\
gpx2shp.cpp\
MLB_Interface.cpp\
GPSBabel.h\
gpx2shp.h\
MLB_Interface.h

libio_gps_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

