if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD -D_SAGA_UNICODE
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version
pkglib_LTLIBRARIES = libimagery_tools.la
libimagery_tools_la_SOURCES =\
evi.cpp\
image_quality_index.cpp\
Image_VI_Distance.cpp\
Image_VI_Slope.cpp\
landsat_acca.cpp\
landsat_import.cpp\
landsat_scene_import.cpp\
landsat_toar.cpp\
landsat_toar_core.cpp\
local_statistical_measures.cpp\
MLB_Interface.cpp\
pansharpening.cpp\
tasseled_cap.cpp\
textural_features.cpp\
evi.h\
image_quality_index.h\
Image_VI_Distance.h\
Image_VI_Slope.h\
landsat_acca.h\
landsat_import.h\
landsat_scene_import.h\
landsat_toar.h\
landsat_toar_core.h\
local_statistical_measures.h\
MLB_Interface.h\
pansharpening.h\
tasseled_cap.h\
textural_features.h

libimagery_tools_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

