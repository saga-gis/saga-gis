#
# $Id: Makefile.am 1843 2013-09-18 10:49:15Z oconrad $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version
pkglib_LTLIBRARIES = libpj_georeference.la
libpj_georeference_la_SOURCES =\
Collect_Points.cpp\
direct_georeferencing.cpp\
Georef_Engine.cpp\
Georef_Grid.cpp\
georef_grid_move.cpp\
Georef_Shapes.cpp\
MLB_Interface.cpp\
set_grid_georeference.cpp\
Collect_Points.h\
direct_georeferencing.h\
Georef_Engine.h\
Georef_Grid.h\
georef_grid_move.h\
Georef_Shapes.h\
MLB_Interface.h\
set_grid_georeference.h

libpj_georeference_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

