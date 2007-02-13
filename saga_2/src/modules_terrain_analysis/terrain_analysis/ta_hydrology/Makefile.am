#
# $Id: Makefile.am,v 1.6 2007-02-13 12:49:32 tschorr Exp $
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
pkglib_LTLIBRARIES = libta_hydrology.la
libta_hydrology_la_SOURCES =\
BasinGrid.cpp\
CellBalance.cpp\
EdgeContamination.cpp\
Flow.cpp\
Flow_AreaDownslope.cpp\
Flow_AreaUpslope.cpp\
Flow_Distance.cpp\
Flow_Parallel.cpp\
Flow_RecursiveDown.cpp\
Flow_RecursiveUp.cpp\
FlowDepth.cpp\
Helper.cpp\
IsochronesConst.cpp\
IsochronesVar.cpp\
LakeFlood.cpp\
LakeFloodInteractive.cpp\
MLB_Interface.cpp\
SAGA_Wetness_Index.cpp\
Sinuosity.cpp\
SlopeLength.cpp\
TopographicIndices.cpp\
BasinGrid.h\
CellBalance.h\
EdgeContamination.h\
Flow.h\
Flow_AreaDownslope.h\
Flow_AreaUpslope.h\
Flow_BRM.h\
Flow_Distance.h\
Flow_Parallel.h\
Flow_RecursiveDown.h\
Flow_RecursiveUp.h\
FlowDepth.h\
Helper.h\
IsochronesConst.h\
IsochronesVar.h\
LakeFlood.h\
MLB_Interface.h\
SAGA_Wetness_Index.h\
Sinuosity.h\
SlopeLength.h\
TopographicIndices.h

libta_hydrology_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la $(ADD_MLBS)

