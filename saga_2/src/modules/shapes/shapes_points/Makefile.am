DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS) `wx-config --cxxflags`
AM_LDFLAGS         = -shared $(DEP_LFLG)
pkglib_LTLIBRARIES = libshapes_points.la
libshapes_points_la_SOURCES =\
AddCoordinates.cpp\
CountPoints.cpp\
CreatePointGrid.cpp\
DistanceMatrix.cpp\
FitNPointsToShape.cpp\
MLB_Interface.cpp\
Points_From_Lines.cpp\
Points_From_Table.cpp\
AddCoordinates.h\
CountPoints.h\
CreatePointGrid.h\
DistanceMatrix.h\
FitNPointsToShape.h\
MLB_Interface.h\
Points_From_Lines.h\
Points_From_Table.h

libshapes_points_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

#install-exec-hook:
#	pushd .; cd $(pkglibdir); ln -s libshapes_points.so shapes_points.so; popd
