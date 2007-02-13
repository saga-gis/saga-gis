#
# $Id: Makefile.am,v 1.5 2007-02-13 12:49:18 tschorr Exp $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
if SAGA_UNICODE
DEP_DEFS = -D_SAGA_UNICODE
endif
if SAGA_UNICODE
UC_DEFS = -D_SAGA_UNICODE
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEP_DEFS) $(DEF_SAGA) $(UC_DEFS) $(DBGFLAGS)
AM_LDFLAGS         = -fPIC -shared
pkglib_LTLIBRARIES = libgrid_gridding.la
libgrid_gridding_la_SOURCES =\
Interpolation.cpp\
Interpolation_InverseDistance.cpp\
Interpolation_NearestNeighbour.cpp\
Interpolation_Shepard.cpp\
Interpolation_Triangulation.cpp\
MLB_Interface.cpp\
Shapes2Grid.cpp\
Shepard.cpp\
Interpolation.h\
Interpolation_InverseDistance.h\
Interpolation_NearestNeighbour.h\
Interpolation_Shepard.h\
Interpolation_Triangulation.h\
MLB_Interface.h\
Shapes2Grid.h\
Shepard.h

libgrid_gridding_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

