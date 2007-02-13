#
# $Id: Makefile.am,v 1.5 2007-02-13 09:57:53 tschorr Exp $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS)
AM_LDFLAGS         = -fPIC -shared
pkglib_LTLIBRARIES = libgrid_tools.la
libgrid_tools_la_SOURCES =\
Grid_Aggregate.cpp\
Grid_Buffer.cpp\
Grid_Buffer_Proximity.cpp\
Grid_CombineGrids.cpp\
Grid_Completion.cpp\
Grid_ConstantGrid.cpp\
Grid_CropToData.cpp\
Grid_Cut.cpp\
Grid_Fill.cpp\
Grid_Gaps.cpp\
Grid_Gaps_OneCell.cpp\
Grid_InvertNoData.cpp\
Grid_Merge.cpp\
Grid_Orientation.cpp\
Grid_Resample.cpp\
Grid_SortRaster.cpp\
Grid_ThresholdBuffer.cpp\
Grid_Value_Reclassify.cpp\
Grid_Value_Replace.cpp\
Grid_Value_Replace_Interactive.cpp\
Grid_Value_Request.cpp\
Grid_Value_Type.cpp\
GridsFromTableAndGrid.cpp\
MLB_Interface.cpp\
Grid_Aggregate.h\
Grid_Buffer.h\
Grid_Buffer_Proximity.h\
Grid_CombineGrids.h\
Grid_Completion.h\
Grid_ConstantGrid.h\
Grid_CropToData.h\
Grid_Cut.h\
Grid_Fill.h\
Grid_Gaps.h\
Grid_Gaps_OneCell.h\
Grid_InvertNoData.h\
Grid_Merge.h\
Grid_Orientation.h\
Grid_Resample.h\
Grid_SortRaster.h\
Grid_ThresholdBuffer.h\
Grid_Value_Reclassify.h\
Grid_Value_Replace.h\
Grid_Value_Replace_Interactive.h\
Grid_Value_Request.h\
Grid_Value_Type.h\
GridsFromTableAndGrid.h\
MLB_Interface.h

libgrid_tools_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

