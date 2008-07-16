#
# $Id: Makefile.am,v 1.7 2008-07-16 11:47:47 oconrad Exp $
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
pkglib_LTLIBRARIES = libshapes_points.la
libshapes_points_la_SOURCES =\
AddCoordinates.cpp\
Clip_Points.cpp\
CountPoints.cpp\
CreatePointGrid.cpp\
DistanceMatrix.cpp\
FitNPointsToShape.cpp\
MLB_Interface.cpp\
Points_From_Lines.cpp\
Points_From_Table.cpp\
remove_duplicates.cpp\
AddCoordinates.h\
CountPoints.h\
Clip_Points.h\
CreatePointGrid.h\
DistanceMatrix.h\
FitNPointsToShape.h\
MLB_Interface.h\
Points_From_Lines.h\
Points_From_Table.h\
remove_duplicates.h

libshapes_points_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

