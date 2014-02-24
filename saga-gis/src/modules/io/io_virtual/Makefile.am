#
# $Id$
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
if SAGA_UNICODE
DEP_DEFS = -fPIC `wx-config --unicode=yes --static=no --cxxflags` -D_SAGA_UNICODE $(DBGFLAGS)
DEP_LFLG = -fPIC -shared `wx-config --unicode=yes --static=no --libs`
UC_DEFS = -D_SAGA_UNICODE
else
DEP_DEFS = -fPIC `wx-config --unicode=no --static=no --cxxflags` $(DBGFLAGS)
DEP_LFLG = -fPIC -shared `wx-config --unicode=no --static=no --libs`
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DEP_DEFS) $(UC_DEFS) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version $(DEP_LFLG)
pkglib_LTLIBRARIES = libio_virtual.la
libio_virtual_la_SOURCES =\
MLB_Interface.cpp\
pc_tileshape_from_spcvf.cpp\
pc_create_spcvf.cpp\
pc_get_subset_spcvf.cpp\
MLB_Interface.h\
pc_tileshape_from_spcvf.h\
pc_create_spcvf.h\
pc_get_subset_spcvf.h

libio_virtual_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la $(ADD_MLBS)

