#
# $Id: Makefile.am,v 1.5 2007-02-13 12:49:26 tschorr Exp $
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
pkglib_LTLIBRARIES = libio_table_odbc.la
libio_table_odbc_la_SOURCES =\
Get_Table.cpp\
MLB_Interface.cpp\
Get_Table.h\
MLB_Interface.h

libio_table_odbc_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

