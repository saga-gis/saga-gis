if DEBUG
DBGFLAGS = -g -DDEBUG
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS) $(DBGFLAGS)
AM_LDFLAGS         = -shared $(DEP_LFLG)
pkglib_LTLIBRARIES = libio_gps.la
libio_gps_la_SOURCES =\
GPSBabel.cpp\
gpx2shp.cpp\
MLB_Interface.cpp\
GPSBabel.h\
gpx2shp.h\
MLB_Interface.h

libio_gps_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

