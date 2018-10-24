#
# $Id: Makefile.am 1828 2013-09-05 21:36:10Z manfred-e $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = $(DEP_LFLG) $(DEP_LIBS) -shared -lodbc -avoid-version
pkglib_LTLIBRARIES = libdb_odbc.la
libdb_odbc_la_SOURCES =\
get_connection.cpp\
MLB_Interface.cpp\
odbc.cpp\
table.cpp\
get_connection.h\
MLB_Interface.h\
odbc.h\
otlv4.h\
table.h

libdb_odbc_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la
