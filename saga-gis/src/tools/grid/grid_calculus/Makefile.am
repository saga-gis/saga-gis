#
# $Id$
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD -D_SAGA_UNICODE
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version
pkglib_LTLIBRARIES = libgrid_calculus.la
libgrid_calculus_la_SOURCES =\
Fuzzify.cpp\
FuzzyAND.cpp\
FuzzyOR.cpp\
./../grid_filter/geodesic_morph_rec/errfunc.c\
./../grid_filter/geodesic_morph_rec/spezfunc.c\
./../grid_filter/geodesic_morph_rec/storeorg.c\
gradient_cartes_polar.cpp\
grid_metric_conversion.cpp\
Grid_Calculator.cpp\
grid_difference.cpp\
Grid_Geometric_Figures.cpp\
Grid_Normalise.cpp\
Grid_Plotter.cpp\
Grid_Random_Field.cpp\
Grid_Random_Terrain.cpp\
Grid_Volume.cpp\
kff_synth.cpp\
MLB_Interface.cpp\
Fuzzify.h\
FuzzyAND.h\
FuzzyOR.h\
./../grid_filter/geodesic_morph_rec/errfunc.h\
./../grid_filter/geodesic_morph_rec/rho.h\
./../grid_filter/geodesic_morph_rec/spezfunc.h\
./../grid_filter/geodesic_morph_rec/storeorg.h\
gradient_cartes_polar.h\
grid_metric_conversion.h\
Grid_Calculator.h\
grid_difference.h\
Grid_Geometric_Figures.h\
Grid_Normalise.h\
Grid_Plotter.h\
Grid_Random_Field.h\
Grid_Random_Terrain.h\
Grid_Volume.h\
kff_synth.h

libgrid_calculus_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

