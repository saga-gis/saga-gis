#
# $Id: Makefile.am,v 1.7 2009-09-16 12:11:54 oconrad Exp $
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
pkglib_LTLIBRARIES = libio_table_odbc.la
libio_table_odbc_la_SOURCES =\
Get_Connection.cpp\
MLB_Interface.cpp\
odbc.cpp\
PGIS_Shapes.cpp\
Shapes.cpp\
Table.cpp\
Get_Connection.h\
MLB_Interface.h\
odbc.h\
PGIS_Shapes.h\
Shapes.h\
Table.h

libio_table_odbc_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

