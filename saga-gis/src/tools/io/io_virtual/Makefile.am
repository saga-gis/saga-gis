#
# $Id$
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEP_DEFS = -fPIC `wx-config --unicode=yes --static=no --cxxflags`
DEP_LFLG = -fPIC -shared `wx-config --unicode=yes --static=no --libs`

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD -D_SAGA_UNICODE
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DEP_DEFS) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version $(DEP_LFLG)
pkglib_LTLIBRARIES = libio_virtual.la
libio_virtual_la_SOURCES =\
MLB_Interface.cpp\
pc_tileshape_from_spcvf.cpp\
pc_create_spcvf.cpp\
pc_get_grid_spcvf.cpp\
pc_get_subset_spcvf.cpp\
pc_remove_overlap_from_spcvf.cpp\
MLB_Interface.h\
pc_tileshape_from_spcvf.h\
pc_create_spcvf.h\
pc_get_grid_spcvf.h\
pc_get_subset_spcvf.h\
pc_remove_overlap_from_spcvf.h

libio_virtual_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la $(ADD_MLBS)
