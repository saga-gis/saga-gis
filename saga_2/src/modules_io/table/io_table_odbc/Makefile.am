DEP_INCS = -I/usr/local/lib/wx
DEP_DEFS = `wx-config --cxxflags`
DEP_LFLG = `wx-config --cxxflags`

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS)
AM_LDFLAGS         = -shared $(DEP_LFLG)
pkglib_LTLIBRARIES = libio_table_odbc.la
libio_table_odbc_la_SOURCES =\
Get_Table.cpp\
MLB_Interface.cpp\
Get_Table.h\
MLB_Interface.h

libio_table_odbc_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

#install-exec-hook:
#	pushd .; cd $(pkglibdir); ln -s libio_table_odbc.so io_table_odbc.so; popd
