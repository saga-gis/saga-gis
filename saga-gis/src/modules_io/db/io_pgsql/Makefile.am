#
# $Id: Makefile.am 1790 2013-08-20 09:23:17Z reklov_w $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
if SAGA_UNICODE
DEP_DEFS = -D_SAGA_UNICODE
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core $(POSTGRESQL_CFLAGS)
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_DEFS) $(DBGFLAGS) -fpermissive $(GOMPFLAGS)
AM_LDFLAGS         = $(DEP_LFLG) $(POSTGRESQL_LDFLAGS) $(DEP_LIBS) -shared -lpq -avoid-version
pkglib_LTLIBRARIES = libio_pgsql.la
libio_pgsql_la_SOURCES =\
get_connection.cpp\
MLB_Interface.cpp\
io_pgsql.cpp\
pgis_raster.cpp\
pgis_shapes.cpp\
table.cpp\
get_connection.h\
MLB_Interface.h\
io_pgsql.h\
pgis_raster.h\
pgis_shapes.h\
table.h

libio_pgsql_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la
