#
# $Id: Makefile.am 1824 2013-09-05 08:48:21Z oconrad $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD -D_SAGA_UNICODE
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version 
pkglib_LTLIBRARIES = libta_hydrology.la
libta_hydrology_la_SOURCES =\
CellBalance.cpp\
EdgeContamination.cpp\
Erosion_LS_Fields.cpp\
Flow.cpp\
Flow_AreaDownslope.cpp\
Flow_AreaUpslope.cpp\
flow_by_slope.cpp\
Flow_Distance.cpp\
Flow_Length.cpp\
Flow_Parallel.cpp\
Flow_RecursiveDown.cpp\
Flow_RecursiveUp.cpp\
flow_massflux.cpp\
flow_width.cpp\
FlowDepth.cpp\
Helper.cpp\
IsochronesConst.cpp\
IsochronesVar.cpp\
LakeFlood.cpp\
LakeFloodInteractive.cpp\
MLB_Interface.cpp\
melton_ruggedness.cpp\
SAGA_Wetness_Index.cpp\
Sinuosity.cpp\
SlopeLength.cpp\
TopographicIndices.cpp\
CellBalance.h\
EdgeContamination.h\
Erosion_LS_Fields.h\
Flow.h\
Flow_AreaDownslope.h\
Flow_AreaUpslope.h\
flow_by_slope.h\
Flow_BRM.h\
Flow_Distance.h\
Flow_Length.h\
Flow_Parallel.h\
Flow_RecursiveDown.h\
Flow_RecursiveUp.h\
flow_massflux.h\
flow_width.h\
FlowDepth.h\
Helper.h\
IsochronesConst.h\
IsochronesVar.h\
LakeFlood.h\
MLB_Interface.h\
melton_ruggedness.h\
SAGA_Wetness_Index.h\
Sinuosity.h\
SlopeLength.h\
TopographicIndices.h

libta_hydrology_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la $(ADD_MLBS)

