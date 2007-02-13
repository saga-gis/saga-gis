#
# $Id: Makefile.am,v 1.6 2007-02-13 12:49:19 tschorr Exp $
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
pkglib_LTLIBRARIES = libshapes_grid.la
libshapes_grid_la_SOURCES =\
Grid_Classes_To_Shapes.cpp\
Grid_Polygon_Clip.cpp\
Grid_Statistics_AddTo_Polygon.cpp\
Grid_To_Contour.cpp\
Grid_To_Gradient.cpp\
Grid_To_Points.cpp\
Grid_To_Points_Random.cpp\
Grid_Values_AddTo_Points.cpp\
Grid_Values_AddTo_Shapes.cpp\
MLB_Interface.cpp\
Grid_Classes_To_Shapes.h\
Grid_Polygon_Clip.h\
Grid_Statistics_AddTo_Polygon.h\
Grid_To_Contour.h\
Grid_To_Gradient.h\
Grid_To_Points.h\
Grid_To_Points_Random.h\
Grid_Values_AddTo_Points.h\
Grid_Values_AddTo_Shapes.h\
MLB_Interface.h

libshapes_grid_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la $(ADD_MLBS)

