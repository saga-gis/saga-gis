if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version
pkglib_LTLIBRARIES = libimagery_isocluster.la
libimagery_isocluster_la_SOURCES =\
classify_isocluster.cpp\
cluster_isodata.cpp\
MLB_Interface.cpp\
classify_isocluster.h\
cluster_isodata.h

libimagery_isocluster_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

