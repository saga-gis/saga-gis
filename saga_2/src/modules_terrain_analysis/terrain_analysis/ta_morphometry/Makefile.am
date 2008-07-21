#
# $Id: Makefile.am,v 1.7 2008-07-21 09:31:44 oconrad Exp $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
if SAGA_UNICODE
UC_DEFS = -D_SAGA_UNICODE
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(UC_DEFS) $(DBGFLAGS)
AM_LDFLAGS         = -fPIC -shared
pkglib_LTLIBRARIES = libta_morphometry.la
libta_morphometry_la_SOURCES =\
Convergence.cpp\
Convergence_Radius.cpp\
Curvature_Classification.cpp\
Hypsometry.cpp\
MLB_Interface.cpp\
mass_balance_index.cpp\
Morphometry.cpp\
ProtectionIndex.cpp\
RealArea.cpp\
SurfaceSpecificPoints.cpp\
mrvbf.cpp\
distance_gradient.cpp\
Convergence.h\
Convergence_Radius.h\
Curvature_Classification.h\
Hypsometry.h\
MLB_Interface.h\
mass_balance_index.h\
Morphometry.h\
ProtectionIndex.h\
RealArea.h\
SurfaceSpecificPoints.h\
mrvbf.h\
distance_gradient.h

libta_morphometry_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

