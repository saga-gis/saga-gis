DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS) `wx-config --cxxflags`
AM_LDFLAGS         = -shared $(DEP_LFLG)
pkglib_LTLIBRARIES = librecreations_fractals.la
librecreations_fractals_la_SOURCES =\
Bifurcation.cpp\
Gaussian_Landscapes.cpp\
Grid_FractalDimension.cpp\
Mandelbrot.cpp\
MLB_Interface.cpp\
Newton.cpp\
Pythagoras_Tree.cpp\
Bifurcation.h\
Gaussian_Landscapes.h\
Grid_FractalDimension.h\
Mandelbrot.h\
MLB_Interface.h\
Newton.h\
Pythagoras_Tree.h

librecreations_fractals_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

#install-exec-hook:
#	pushd .; cd $(pkglibdir); ln -s librecreations_fractals.so recreations_fractals.so; popd
