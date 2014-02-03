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
pkglib_LTLIBRARIES = libshapes_polygons.la
libshapes_polygons_la_SOURCES =\
add_point_attributes.cpp\
MLB_Interface.cpp\
Polygon_Centroids.cpp\
Polygon_Clip.cpp\
Polygon_Geometrics.cpp\
Polygon_Intersection.cpp\
Polygon_SelfIntersection.cpp\
Polygon_StatisticsFromPoints.cpp\
Polygon_Union.cpp\
Polygons_From_Lines.cpp\
polygon_line_intersection.cpp\
polygon_split_parts.cpp\
polygon_to_edges_nodes.cpp\
polygon_to_points.cpp\
shape_index.cpp\
add_point_attributes.h\
MLB_Interface.h\
Polygon_Centroids.h\
Polygon_Clip.h\
Polygon_Geometrics.h\
Polygon_Intersection.h\
Polygon_SelfIntersection.h\
Polygon_StatisticsFromPoints.h\
Polygon_Union.h\
Polygons_From_Lines.h\
polygon_line_intersection.h\
polygon_split_parts.h\
polygon_to_edges_nodes.h\
polygon_to_points.h\
shape_index.h

libshapes_polygons_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

