DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS)
AM_LDFLAGS         = -shared $(DEP_LFLG) $(DEP_LIBS)
pkglib_LTLIBRARIES = libgrid_analysis.la
libgrid_analysis_la_SOURCES =\
Cost_Anisotropic.cpp\
Cost_Isotropic.cpp\
Cost_PolarToRect.cpp\
Cost_RectToPolar.cpp\
CoveredDistance.cpp\
CrossClassification.cpp\
Fuzzify.cpp\
FuzzyAND.cpp\
FuzzyOR.cpp\
Grid_AggregationIndex.cpp\
Grid_AHP.cpp\
Grid_CVA.cpp\
Grid_LayerOfMaximumValue.cpp\
Grid_Pattern.cpp\
Image_VI_Distance.cpp\
Image_VI_Slope.cpp\
LeastCostPathProfile.cpp\
MLB_Interface.cpp\
owa.cpp\
PointsEx.cpp\
Cost_Anisotropic.h\
Cost_Isotropic.h\
Cost_PolarToRect.h\
Cost_RectToPolar.h\
CoveredDistance.h\
CrossClassification.h\
Fuzzify.h\
FuzzyAND.h\
FuzzyOR.h\
Grid_AggregationIndex.h\
Grid_AHP.h\
Grid_CVA.h\
Grid_LayerOfMaximumValue.h\
Grid_Pattern.h\
Image_VI_Distance.h\
Image_VI_Slope.h\
LeastCostPathProfile.h\
MLB_Interface.h\
owa.h\
PointsEx.h

libgrid_analysis_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la $(ADD_MLBS)

#install-exec-hook:
#	pushd .; cd $(pkglibdir); ln -s libgrid_analysis.so grid_analysis.so; popd
