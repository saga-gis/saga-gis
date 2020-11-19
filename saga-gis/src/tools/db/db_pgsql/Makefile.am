#
# $Id: Makefile.am 1790 2013-08-20 09:23:17Z reklov_w $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core $(POSTGRESQL_CFLAGS)
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS) $(POSTGRESQL_CPPFLAGS)
AM_LDFLAGS         = $(DEP_LFLG) $(POSTGRESQL_LDFLAGS) $(POSTGRESQL_LIBS) $(DEP_LIBS) -shared -avoid-version
pkglib_LTLIBRARIES = libdb_pgsql.la
libdb_pgsql_la_SOURCES =\
get_connection.cpp\
MLB_Interface.cpp\
db_pgsql.cpp\
pgis_raster.cpp\
pgis_shapes.cpp\
table.cpp\
get_connection.h\
MLB_Interface.h\
db_pgsql.h\
pgis_raster.h\
pgis_shapes.h\
table.h

libdb_pgsql_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la
