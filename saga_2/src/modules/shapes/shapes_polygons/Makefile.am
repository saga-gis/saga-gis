if DEBUG
DBGFLAGS = -g -DDEBUG
endif
DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS) $(DBGFLAGS)
AM_LDFLAGS         = -shared $(DEP_LFLG)
pkglib_LTLIBRARIES = libshapes_polygons.la
libshapes_polygons_la_SOURCES =\
MLB_Interface.cpp\
Polygon_Centroids.cpp\
Polygon_Geometrics.cpp\
Polygon_Intersection.cpp\
Polygon_Intersection_GPC.cpp\
Polygon_StatisticsFromPoints.cpp\
Polygons_From_Lines.cpp\
MLB_Interface.h\
Polygon_Centroids.h\
Polygon_Geometrics.h\
Polygon_Intersection.h\
Polygon_Intersection_GPC.h\
Polygon_StatisticsFromPoints.h\
Polygons_From_Lines.h

libshapes_polygons_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

