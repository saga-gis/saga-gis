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
pkglib_LTLIBRARIES = libgrid_analysis.la
libgrid_analysis_la_SOURCES =\
Cost_Isotropic.cpp\
coverage_of_categories.cpp\
CoveredDistance.cpp\
CrossClassification.cpp\
diversity_analysis.cpp\
diversity_raos_q.cpp\
diversity_shannon.cpp\
diversity_simpson.cpp\
fragmentation_base.cpp\
fragmentation_classify.cpp\
fragmentation_resampling.cpp\
fragmentation_standard.cpp\
Grid_Accumulation_Functions.cpp\
Grid_AggregationIndex.cpp\
Grid_AHP.cpp\
Grid_CVA.cpp\
Grid_IMCORR.cpp\
Grid_LayerOfMaximumValue.cpp\
Grid_Pattern.cpp\
LeastCostPathProfile.cpp\
LeastCostPathProfile_Points.cpp\
MLB_Interface.cpp\
owa.cpp\
Soil_Texture.cpp\
Cost_Isotropic.h\
coverage_of_categories.h\
CoveredDistance.h\
CrossClassification.h\
diversity_analysis.h\
diversity_raos_q.h\
diversity_shannon.h\
diversity_simpson.h\
fragmentation_base.h\
fragmentation_classify.h\
fragmentation_resampling.h\
fragmentation_standard.h\
Grid_Accumulation_Functions.h\
Grid_AggregationIndex.h\
Grid_AHP.h\
Grid_CVA.h\
Grid_IMCORR.h\
Grid_LayerOfMaximumValue.h\
Grid_Pattern.h\
LeastCostPathProfile.h\
LeastCostPathProfile_Points.h\
MLB_Interface.h\
owa.h\
Soil_Texture.h

libgrid_analysis_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la $(ADD_MLBS)

