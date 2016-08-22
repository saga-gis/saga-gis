#
# $Id: Makefile.am 1896 2013-12-10 14:56:59Z oconrad $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

if HAVE_HDF5
DEF_HDF5=-DWITH_HDF5
endif

if HAVE_VIGRA
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD -D_SAGA_UNICODE $(DEF_HDF5)
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS) $(HDF5_CFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version -lvigraimpex -lfftw3
pkglib_LTLIBRARIES = libimagery_vigra.la
libimagery_vigra_la_SOURCES =\
vigra.h\
vigra_edges.h\
vigra_smoothing.h\
vigra_distance.h\
vigra_fft.h\
vigra_morphology.h\
vigra_random_forest.h\
vigra_watershed.h\
MLB_Interface.cpp\
vigra.cpp\
MLB_Interface.h\
vigra_distance.cpp\
vigra_fft.cpp\
vigra_smoothing.cpp\
vigra_edges.cpp\
vigra_morphology.cpp\
vigra_random_forest.cpp\
vigra_watershed.cpp

libimagery_vigra_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la
endif
