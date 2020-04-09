if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version
pkglib_LTLIBRARIES = libsim_air_flow.la
libsim_air_flow_la_SOURCES =\
cold_air_flow.cpp\
TLB_Interface.cpp\
cold_air_flow.h

libsim_air_flow_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

