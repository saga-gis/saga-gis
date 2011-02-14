#
# $Id: Makefile.am,v 1.1 2010-11-26 13:31:17 oconrad Exp $
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
pkglib_LTLIBRARIES = libdev_tools.la
libdev_tools_la_SOURCES =\
MLB_Interface.cpp\
tl_extract.cpp\
tl_merge.cpp\
tl_union.cpp\
MLB_Interface.h\
tl_extract.h\
tl_merge.h\
tl_union.h

libdev_tools_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

