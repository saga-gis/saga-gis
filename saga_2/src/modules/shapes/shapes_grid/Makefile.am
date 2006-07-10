DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS)
AM_LDFLAGS         = -shared $(DEP_LFLG) $(DEP_LIBS)
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

#install-exec-hook:
#	pushd .; cd $(pkglibdir); ln -s libshapes_grid.so shapes_grid.so; popd
