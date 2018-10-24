if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version
pkglib_LTLIBRARIES = libsim_geomorphology.la
libsim_geomorphology_la_SOURCES =\
GPP_Model.cpp\
GPP_Model_BASE.cpp\
MLB_Interface.cpp\
GPP_Model.h\
GPP_Model_BASE.h\
MLB_Interface.h

libsim_geomorphology_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

