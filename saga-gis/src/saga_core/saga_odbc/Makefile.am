if DEBUG
DBGFLAGS = -g -DDEBUG
endif
if SAGA_UNICODE
DEP_DEFS = -D_SAGA_UNICODE
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_DEFS) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = $(DEP_LFLG) -lodbc
lib_LTLIBRARIES    = libsaga_odbc.la
libsaga_odbc_la_LDFLAGS =$(AM_LDFLAGS) -release $(VERSION)
libsaga_odbc_la_SOURCES =\
saga_odbc.cpp\
saga_odbc.h\
otlv4.h

libsaga_odbc_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la
