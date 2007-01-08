DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS) `wx-config --cxxflags`
AM_LDFLAGS         = -shared $(DEP_LFLG)
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

#install-exec-hook:
#	pushd .; cd $(pkglibdir); ln -s libgrid_calculus.so grid_calculus.so; popd
