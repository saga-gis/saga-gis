DEF_SAGA           = -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD
CXX_INCS           = -I$(top_srcdir)/src/saga_core
AM_CXXFLAGS        = $(CXX_INCS) $(DEF_SAGA) $(DEP_INCS) $(DEP_DEFS)
AM_LDFLAGS         = -shared $(DEP_LFLG)
pkglib_LTLIBRARIES = libshapes_tools.la
libshapes_tools_la_SOURCES =\
CreateChartLayer.cpp\
CreateWebContent.cpp\
GraticuleBuilder.cpp\
Intersection_GPC.c\
MLB_Interface.cpp\
NewLayerFromSelectedShapes.cpp\
QueryBuilder.cpp\
QueryParser.cpp\
SearchInTable.cpp\
SelectByTheme.cpp\
SeparateShapes.cpp\
Shapes_Assign_Table.cpp\
Shapes_Create_Empty.cpp\
Shapes_Merge.cpp\
Shapes_Report.cpp\
ShapeSearchSelector.cpp\
ShapeSelector.cpp\
Summarize.cpp\
SummaryPDFDocEngine.cpp\
TransformShapes.cpp\
CreateChartLayer.h\
CreateWebContent.h\
GraticuleBuilder.h\
Intersection_GPC.h\
MLB_Interface.h\
NewLayerFromSelectedShapes.h\
QueryBuilder.h\
QueryParser.h\
SearchInTable.h\
SelectByTheme.h\
SeparateShapes.h\
Shapes_Assign_Table.h\
Shapes_Create_Empty.h\
Shapes_Merge.h\
Shapes_Report.h\
ShapeSearchSelector.h\
ShapeSelector.h\
Summarize.h\
SummaryPDFDocEngine.h\
TransformShapes.h

libshapes_tools_la_LIBADD = $(top_srcdir)/src/saga_core/saga_api/libsaga_api.la

#install-exec-hook:
#	pushd .; cd $(pkglibdir); ln -s libshapes_tools.so shapes_tools.so; popd
