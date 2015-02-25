#
# $Id: Makefile.am 1619 2013-03-05 16:07:00Z oconrad $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
if SAGA_UNICODE
UC_DEFS = -D_SAGA_UNICODE
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(UC_DEFS) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version
pkglib_LTLIBRARIES = libta_morphometry.la
libta_morphometry_la_SOURCES =\
Convergence.cpp\
Convergence_Radius.cpp\
Curvature_Classification.cpp\
Curvature_UpDownSlope.cpp\
Hypsometry.cpp\
MLB_Interface.cpp\
Morphometry.cpp\
ProtectionIndex.cpp\
RealArea.cpp\
SurfaceSpecificPoints.cpp\
air_flow_height.cpp\
anisotropic_heating.cpp\
distance_gradient.cpp\
fuzzy_landform_elements.cpp\
land_surface_temperature.cpp\
mass_balance_index.cpp\
mrvbf.cpp\
param_scale.cpp\
relative_heights.cpp\
ruggedness.cpp\
tc_iwahashi_pike.cpp\
top_hat.cpp\
tpi.cpp\
wind_effect.cpp\
Convergence.h\
Convergence_Radius.h\
Curvature_Classification.h\
Curvature_UpDownSlope.h\
Hypsometry.h\
MLB_Interface.h\
Morphometry.h\
ProtectionIndex.h\
RealArea.h\
SurfaceSpecificPoints.h\
air_flow_height.h\
anisotropic_heating.h\
distance_gradient.h\
fuzzy_landform_elements.h\
land_surface_temperature.h\
mass_balance_index.h\
mrvbf.h\
param_scale.h\
relative_heights.h\
ruggedness.h\
tc_iwahashi_pike.h\
top_hat.h\
tpi.h\
wind_effect.h

libta_morphometry_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

