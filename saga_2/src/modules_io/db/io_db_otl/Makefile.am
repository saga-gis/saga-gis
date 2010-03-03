#
# $Id: Makefile.am,v 1.1 2010-03-03 19:09:54 oconrad Exp $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
if SAGA_UNICODE
DEP_DEFS = -fPIC `wx-config --unicode=yes --static=no --cxxflags` -D_SAGA_UNICODE $(DBGFLAGS)
DEP_LFLG = -fPIC -shared `wx-config --unicode=yes --static=no --libs`
else
DEP_DEFS = -fPIC `wx-config --unicode=no --static=no --cxxflags` $(DBGFLAGS)
DEP_LFLG = -fPIC -shared `wx-config --unicode=no --static=no --libs`
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_DEFS) $(DBGFLAGS)
AM_LDFLAGS         = $(DEP_LFLG)
pkglib_LTLIBRARIES = libio_db_odbc.la
libio_db_odbc_la_SOURCES =\
get_connection.cpp\
MLB_Interface.cpp\
odbc.cpp\
pgis_shapes.cpp\
shapes.cpp\
table.cpp\
get_connection.h\
MLB_Interface.h\
odbc.h\
otlv4.h\
pgis_shapes.h\
shapes.h\
table.h

libio_db_odbc_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

