#
# $Id$
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
if SAGA_UNICODE
UC_DEFS = -D_SAGA_UNICODE
endif
if HAVE_VIGRA
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(UC_DEFS) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version -lvigraimpex -lfftw3
pkglib_LTLIBRARIES = libvigra.la
libvigra_la_SOURCES =\
vigra.h\
vigra_edges.h\
vigra_smoothing.h\
vigra_distance.h\
vigra_fft.h\
vigra_morphology.h\
vigra_watershed.h\
MLB_Interface.cpp\
vigra.cpp\
MLB_Interface.h\
vigra_distance.cpp\
vigra_fft.cpp\
vigra_smoothing.cpp\
vigra_edges.cpp\
vigra_morphology.cpp\
vigra_watershed.cpp

libvigra_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la
endif
