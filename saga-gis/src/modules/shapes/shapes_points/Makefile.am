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
pkglib_LTLIBRARIES = libshapes_points.la
libshapes_points_la_SOURCES =\
add_polygon_attributes.cpp\
AddCoordinates.cpp\
Clip_Points.cpp\
convex_hull.cpp\
CountPoints.cpp\
CreatePointGrid.cpp\
DistanceMatrix.cpp\
FitNPointsToShape.cpp\
gps_track_aggregation.cpp\
MLB_Interface.cpp\
points_filter.cpp\
Points_From_Lines.cpp\
Points_From_Table.cpp\
points_thinning.cpp\
remove_duplicates.cpp\
select_points.cpp\
separate_by_direction.cpp\
snap_points_to_features.cpp\
snap_points_to_grid.cpp\
thiessen_polygons.cpp\
add_polygon_attributes.h\
AddCoordinates.h\
convex_hull.h\
CountPoints.h\
Clip_Points.h\
CreatePointGrid.h\
DistanceMatrix.h\
FitNPointsToShape.h\
gps_track_aggregation.h\
MLB_Interface.h\
points_filter.h\
Points_From_Lines.h\
Points_From_Table.h\
points_thinning.h\
remove_duplicates.h\
select_points.h\
separate_by_direction.h\
snap_points_to_features.h\
snap_points_to_grid.h\
thiessen_polygons.h

libshapes_points_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

