#
# $Id: Makefile.am,v 1.7 2008-04-22 09:43:54 oconrad Exp $
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
pkglib_LTLIBRARIES = libshapes_polygons.la
libshapes_polygons_la_SOURCES =\
MLB_Interface.cpp\
Polygon_Centroids.cpp\
Polygon_Geometrics.cpp\
Polygon_Intersection.cpp\
Polygon_Clipper.cpp\
Polygon_StatisticsFromPoints.cpp\
Polygon_Union.cpp\
Polygons_From_Lines.cpp\
MLB_Interface.h\
Polygon_Centroids.h\
Polygon_Geometrics.h\
Polygon_Intersection.h\
Polygon_Clipper.h\
Polygon_StatisticsFromPoints.h\
Polygon_Union.h\
Polygons_From_Lines.h

libshapes_polygons_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

