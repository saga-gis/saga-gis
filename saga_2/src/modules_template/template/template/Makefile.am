if DEBUG
DBGFLAGS = -g -DDEBUG
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS) $(DBGFLAGS)
AM_LDFLAGS         = -shared $(DEP_LFLG)
pkglib_LTLIBRARIES = libTemplate.la
libTemplate_la_SOURCES =\
Grid_IO_GDAL.cpp\
MLB_Interface.cpp\
Grid_IO_GDAL.h\
MLB_Interface.h

libTemplate_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

