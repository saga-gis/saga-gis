#
# $Id$
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = -fPIC $(CXX_INCS) $(DEF_SAGA) $(DBGFLAGS) $(GOMPFLAGS)
AM_LDFLAGS         = -fPIC -shared -avoid-version
pkglib_LTLIBRARIES = libshapes_tools.la
libshapes_tools_la_SOURCES =\
beachball.cpp\
CreateChartLayer.cpp\
GraticuleBuilder.cpp\
LandUse_Scenario.cpp\
MLB_Interface.cpp\
NewLayerFromSelectedShapes.cpp\
quadtree_structure.cpp\
QueryBuilder.cpp\
SearchInTable.cpp\
SelectByTheme.cpp\
SeparateShapes.cpp\
Shapes_Create_Empty.cpp\
Shapes_Merge.cpp\
Shapes_Support_Tool_Chains.cpp\
TransformShapes.cpp\
shapes_buffer.cpp\
shapes_clean.cpp\
shapes_convert_vertex_type.cpp\
shapes_cut.cpp\
shapes_cut_interactive.cpp\
shapes_extents.cpp\
shapes_generate.cpp\
shapes_polar_to_cartes.cpp\
shapes_split.cpp\
shapes_split_by_attribute.cpp\
shapes_split_randomly.cpp\
visualize_gradients.cpp\
beachball.h\
CreateChartLayer.h\
GraticuleBuilder.h\
LandUse_Scenario.h\
MLB_Interface.h\
NewLayerFromSelectedShapes.h\
quadtree_structure.h\
QueryBuilder.h\
SearchInTable.h\
SelectByTheme.h\
SeparateShapes.h\
Shapes_Create_Empty.h\
Shapes_Merge.h\
Shapes_Support_Tool_Chains.h\
TransformShapes.h\
shapes_buffer.h\
shapes_clean.h\
shapes_convert_vertex_type.h\
shapes_cut.h\
shapes_cut_interactive.h\
shapes_extents.h\
shapes_generate.h\
shapes_polar_to_cartes.h\
shapes_split.h\
shapes_split_by_attribute.h\
shapes_split_randomly.h\
visualize_gradients.h

libshapes_tools_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

#install-exec-hook:
#	pushd .; cd $(pkglibdir); ln -s libshapes_tools.so shapes_tools.so; popd
