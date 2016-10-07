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
pkglib_LTLIBRARIES = libshapes_grid.la
libshapes_grid_la_SOURCES =\
Grid_Class_Statistics_For_Polygons.cpp\
Grid_Classes_To_Shapes.cpp\
grid_extent.cpp\
grid_local_extremes_to_points.cpp\
Grid_Polygon_Clip.cpp\
grid_rectangle_clip.cpp\
Grid_Statistics_AddTo_Polygon.cpp\
Grid_Statistics_For_Points.cpp\
Grid_To_Contour.cpp\
Grid_To_Gradient.cpp\
Grid_To_Points.cpp\
Grid_To_Points_Random.cpp\
Grid_Values_AddTo_Points.cpp\
Grid_Values_AddTo_Shapes.cpp\
MLB_Interface.cpp\
Grid_Class_Statistics_For_Polygons.h\
Grid_Classes_To_Shapes.h\
grid_extent.h\
grid_local_extremes_to_points.h\
Grid_Polygon_Clip.h\
grid_rectangle_clip.h\
Grid_Statistics_AddTo_Polygon.h\
Grid_Statistics_For_Points.h\
Grid_To_Contour.h\
Grid_To_Gradient.h\
Grid_To_Points.h\
Grid_To_Points_Random.h\
Grid_Values_AddTo_Points.h\
Grid_Values_AddTo_Shapes.h\
MLB_Interface.h

libshapes_grid_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la $(ADD_MLBS)

