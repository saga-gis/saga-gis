#
# $Id$
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version
pkglib_LTLIBRARIES = libshapes_lines_ex.la
libshapes_lines_ex_la_SOURCES =\
MLB_Interface.cpp\
SimplifyShapes.cpp\
Containers.h\
DPHull.h\
LineApproximator.h\
MLB_Interface.h\
PathHull.h\
SimplifyShapes.h

libshapes_lines_ex_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

