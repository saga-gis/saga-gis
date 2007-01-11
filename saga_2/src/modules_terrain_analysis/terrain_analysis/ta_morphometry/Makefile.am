if DEBUG
DBGFLAGS = -g -DDEBUG
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS) $(DBGFLAGS)
AM_LDFLAGS         = -shared $(DEP_LFLG)
pkglib_LTLIBRARIES = libta_morphometry.la
libta_morphometry_la_SOURCES =\
Convergence.cpp\
Convergence_Radius.cpp\
Curvature_Classification.cpp\
Hypsometry.cpp\
MLB_Interface.cpp\
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
Morphometry.h\
ProtectionIndex.h\
RealArea.h\
SurfaceSpecificPoints.h\
mrvbf.h\
distance_gradient.h

libta_morphometry_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

