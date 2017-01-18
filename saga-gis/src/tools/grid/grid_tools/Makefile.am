#
# $Id$
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD -D_SAGA_UNICODE
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version
pkglib_LTLIBRARIES = libgrid_tools.la
libgrid_tools_la_SOURCES =\
CreateGridSystem.cpp\
Grid_Aggregate.cpp\
Grid_Buffer.cpp\
Grid_Buffer_Proximity.cpp\
Grid_Combine_Classes.cpp\
Grid_CombineGrids.cpp\
Grid_Completion.cpp\
Grid_ConstantGrid.cpp\
Grid_CropToData.cpp\
Grid_Cut.cpp\
Grid_Fill.cpp\
Grid_Gaps.cpp\
Grid_Gaps_OneCell.cpp\
Grid_Gaps_Resampling.cpp\
Grid_Gaps_Spline_Fill.cpp\
Grid_InvertNoData.cpp\
Grid_Mask.cpp\
Grid_Merge.cpp\
Grid_Orientation.cpp\
Grid_Proximity.cpp\
Grid_Resample.cpp\
Grid_Shrink_Expand.cpp\
Grid_SortRaster.cpp\
Grid_Support_Tool_Chains.cpp\
Grid_ThresholdBuffer.cpp\
Grid_Tiling.cpp\
Grid_Transpose.cpp\
Grid_Value_NoData.cpp\
Grid_Value_Reclassify.cpp\
Grid_Value_Replace.cpp\
Grid_Value_Replace_Interactive.cpp\
Grid_Value_Request.cpp\
Grid_Value_Type.cpp\
GridsFromTableAndGrid.cpp\
MLB_Interface.cpp\
CreateGridSystem.h\
Grid_Aggregate.h\
Grid_Buffer.h\
Grid_Buffer_Proximity.h\
Grid_Combine_Classes.h\
Grid_CombineGrids.h\
Grid_Completion.h\
Grid_ConstantGrid.h\
Grid_CropToData.h\
Grid_Cut.h\
Grid_Fill.h\
Grid_Gaps.h\
Grid_Gaps_OneCell.h\
Grid_Gaps_Resampling.h\
Grid_Gaps_Spline_Fill.h\
Grid_InvertNoData.h\
Grid_Mask.h\
Grid_Merge.h\
Grid_Orientation.h\
Grid_Proximity.h\
Grid_Resample.h\
Grid_Shrink_Expand.h\
Grid_SortRaster.h\
Grid_Support_Tool_Chains.h\
Grid_ThresholdBuffer.h\
Grid_Tiling.h\
Grid_Transpose.h\
Grid_Value_NoData.h\
Grid_Value_Reclassify.h\
Grid_Value_Replace.h\
Grid_Value_Replace_Interactive.h\
Grid_Value_Request.h\
Grid_Value_Type.h\
GridsFromTableAndGrid.h\
MLB_Interface.h

libgrid_tools_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

