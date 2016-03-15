if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEP_DEFS = -fPIC `wx-config --unicode=yes --static=no --cxxflags`
DEP_LFLG = -fPIC -shared `wx-config --unicode=yes --static=no --libs`

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD -D_SAGA_UNICODE
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_DEFS) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = $(DEP_LFLG) -avoid-version
pkglib_LTLIBRARIES = libio_grid_image.la
libio_grid_image_la_SOURCES =\
grid_export.cpp\
grid_import.cpp\
grid_to_kml.cpp\
MLB_Interface.cpp\
grid_export.h\
grid_import.h\
grid_to_kml.h\
MLB_Interface.h
 
libio_grid_image_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

