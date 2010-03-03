#
# $Id: Makefile.am,v 1.1 2010-03-03 19:09:54 oconrad Exp $
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
pkglib_LTLIBRARIES = libio_db_mysql.la
libio_db_mysql_la_SOURCES =\
Get_Table.cpp\
MLB_Interface.cpp\
Get_Table.h\
MLB_Interface.h

libio_db_mysql_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

