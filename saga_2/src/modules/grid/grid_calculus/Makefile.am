#
# $Id: Makefile.am,v 1.4 2007-02-13 09:57:49 tschorr Exp $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS)
AM_LDFLAGS         = -fPIC -shared
pkglib_LTLIBRARIES = libgrid_calculus.la
libgrid_calculus_la_SOURCES =\
Grid_Calculator.cpp\
Grid_Geometric_Figures.cpp\
Grid_Normalise.cpp\
Grid_Plotter.cpp\
Grid_Random_Field.cpp\
Grid_Random_Terrain.cpp\
Grid_Volume.cpp\
MLB_Interface.cpp\
Grid_Calculator.h\
Grid_Geometric_Figures.h\
Grid_Normalise.h\
Grid_Plotter.h\
Grid_Random_Field.h\
Grid_Random_Terrain.h\
Grid_Volume.h\
MLB_Interface.h

libgrid_calculus_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

