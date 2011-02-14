#
# $Id$
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
if SAGA_UNICODE
DEP_DEFS = -D_SAGA_UNICODE
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DEP_DEFS) $(DBGFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version
pkglib_LTLIBRARIES = libpj_georeference.la
libpj_georeference_la_SOURCES =\
Collect_Points.cpp\
Georef_Engine.cpp\
Georef_Grid.cpp\
georef_grid_move.cpp\
Georef_Shapes.cpp\
MLB_Interface.cpp\
enorm.c\
fdjac2.c\
lmdif.c\
lmdif0.c\
lmpar.c\
qrfac.c\
qrsolv.c\
Collect_Points.h\
Georef_Engine.h\
Georef_Grid.h\
georef_grid_move.h\
Georef_Shapes.h\
MLB_Interface.h\
cminpak.h\
dpmpar.h

libpj_georeference_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

