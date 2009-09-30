#
# $Id: Makefile.am,v 1.1 2009-09-30 18:00:35 oconrad Exp $
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
AM_LDFLAGS         = -fPIC -shared
pkglib_LTLIBRARIES = libvigra.la
libvigra_la_SOURCES =\
MLB_Interface.cpp\
vigra.cpp\
MLB_Interface.h\
vigra.h

libvigra_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

