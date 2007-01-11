if DEBUG
DBGFLAGS = -g -DDEBUG
endif
DEP_DEFS = `wx-config --unicode=no --cxxflags`
DEP_LFLG = `wx-config --unicode=no --libs`
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_DEFS) $(DBGFLAGS) `wx-config --cxxflags`
AM_LDFLAGS         = -shared $(DEP_LFLG)
pkglib_LTLIBRARIES = libio_grid_image.la
libio_grid_image_la_SOURCES =\
grid_export.cpp\
grid_import.cpp\
MLB_Interface.cpp\
grid_export.h\
grid_import.h\
MLB_Interface.h
 
libio_grid_image_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

