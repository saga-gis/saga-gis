#
# $Id$
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
pkglib_LTLIBRARIES = libgrid_visualisation.la
libgrid_visualisation_la_SOURCES =\
Grid_3D_Image.cpp\
Grid_Aspect_Slope_Map.cpp\
Grid_Color_Blend.cpp\
Grid_Color_Triangle.cpp\
Grid_Color_Rotate.cpp\
Grid_Colors_Fit.cpp\
Grid_Histogram_Surface.cpp\
Grid_RGB_Composite.cpp\
Grid_Terrain_Map.cpp\
MLB_Interface.cpp\
Grid_3D_Image.h\
Grid_Aspect_Slope_Map.h\
Grid_Color_Blend.h\
Grid_Color_Rotate.h\
Grid_Color_Triangle.h\
Grid_Colors_Fit.h\
Grid_Histogram_Surface.h\
Grid_RGB_Composite.h\
Grid_Terrain_Map.h\
MLB_Interface.h

libgrid_visualisation_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

