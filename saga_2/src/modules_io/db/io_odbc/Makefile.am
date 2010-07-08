#
# $Id: Makefile.am,v 1.3 2010-07-08 15:08:36 johanvdw Exp $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
if SAGA_UNICODE
DEP_DEFS = -D_SAGA_UNICODE
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_DEFS) $(DBGFLAGS)
AM_LDFLAGS         = $(DEP_LFLG) $(DEP_LIBS) -avoid-version
pkglib_LTLIBRARIES = libio_odbc.la
libio_odbc_la_SOURCES =\
get_connection.cpp\
MLB_Interface.cpp\
pgis_shapes.cpp\
shapes.cpp\
table.cpp\
get_connection.h\
MLB_Interface.h\
pgis_shapes.h\
shapes.h\
table.h

libio_odbc_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la $(top_srcdir)/src/saga_core/saga_odbc/libsaga_odbc.la

