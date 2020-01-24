if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEP_DEFS = -fPIC `wx-config --unicode=yes --static=no --cxxflags` `curl-config --cflags`
DEP_LFLG = -fPIC -shared `wx-config --unicode=yes --static=no --libs` `curl-config --libs`

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_DEFS) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = $(DEP_LFLG) -avoid-version
pkglib_LTLIBRARIES = libio_webservices.la
libio_webservices_la_SOURCES =\
geocoding.cpp\
sg_curl.cpp\
TLB_Interface.cpp\
geocoding.h\
sg_curl.h

libio_webservices_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la
