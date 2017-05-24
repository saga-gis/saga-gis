#
# $Id:
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD -D_SAGA_UNICODE
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version 
pkglib_LTLIBRARIES = libgrids_tools.la
libgrids_tools_la_SOURCES =\
grid_collection.cpp\
TLB_Interface.cpp\
grid_collection.h

libgrids_tools_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

