#
# $Id$
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif

if SAGA_PYTHON
SAGAPYTHONFLAGS= -D_SAGA_PYTHON
endif

AM_CXXFLAGS = -fPIC -Wall `wx-config --unicode=yes --cxxflags base` -D_SAGA_LINUX -D_SAGA_UNICODE -D_TYPEDEF_BYTE -D_TYPEDEF_WORD -D_SAGA_API_EXPORTS $(DISABLELIBHARU) $(DBGFLAGS) $(SAGAPYTHONFLAGS) $(GOMPFLAGS)
AM_LDFLAGS = -fPIC -shared -Wall `wx-config --unicode=yes --libs base,net,xml,html` $(HARU_LIB) $(SAGAPYTHONFLAGS)

libsaga_api_la_LDFLAGS =$(AM_LDFLAGS) -release $(VERSION)

lib_LTLIBRARIES = libsaga_api.la

otherincludedir = $(pkgincludedir)/saga_core/saga_api
otherinclude_HEADERS = api_core.h \
data_manager.h \
dataobject.h \
datetime.h \
geo_tools.h \
grid.h \
grids.h \
grid_pyramid.h \
mat_tools.h \
metadata.h \
parameters.h \
pointcloud.h \
saga_api.h \
shapes.h \
table_dbase.h \
table.h \
table_value.h \
tin.h \
tool.h \
tool_library.h

libsaga_api_la_SOURCES =\
api_callback.cpp\
api_colors.cpp\
api_core.cpp\
api_file.cpp\
api_memory.cpp\
api_string.cpp\
api_translator.cpp\
clipper.cpp\
data_manager.cpp\
dataobject.cpp\
datetime.cpp\
geo_classes.cpp\
geo_functions.cpp\
grid.cpp\
grids.cpp \
grid_io.cpp\
grid_memory.cpp\
grid_operation.cpp\
grid_pyramid.cpp\
grid_system.cpp\
mat_formula.cpp\
mat_grid_radius.cpp\
mat_indexing.cpp\
mat_matrix.cpp\
mat_regression.cpp\
mat_regression_multiple.cpp\
mat_regression_weighted.cpp\
mat_mRMR.cpp\
mat_spline.cpp\
mat_tools.cpp\
mat_trend.cpp\
metadata.cpp\
parameter.cpp\
parameter_data.cpp\
parameters.cpp\
pointcloud.cpp\
projections.cpp\
quadtree.cpp\
saga_api.cpp\
shape.cpp\
shape_line.cpp\
shape_part.cpp\
shape_point.cpp\
shape_points.cpp\
shape_polygon.cpp\
shapes.cpp\
shapes_io.cpp\
shapes_ogis.cpp\
shapes_polygons.cpp\
shapes_search.cpp\
shapes_selection.cpp\
table.cpp\
table_dbase.cpp\
table_io.cpp\
table_record.cpp\
table_selection.cpp\
tin.cpp\
tin_elements.cpp\
tin_triangulation.cpp\
tool.cpp\
tool_chain.cpp\
tool_grid.cpp\
tool_grid_interactive.cpp\
tool_interactive.cpp\
tool_interactive_base.cpp\
tool_library.cpp\
tool_library_interface.cpp\
tool_summary.cpp\
api_core.h\
clipper.hpp\
data_manager.h\
dataobject.h\
datetime.h\
geo_tools.h\
grid.h\
grid_pyramid.h\
mat_tools.h\
metadata.h\
parameters.h\
pointcloud.h\
saga_api.h\
shapes.h\
table.h\
table_dbase.h\
table_value.h\
tin.h\
tool.h\
tool_chain.h\
tool_library.h

install-exec-hook:
	-ldconfig


if SAGA_PYTHON
SWIGUNICODEFLAGS = -D_SWIG_WCHAR -D_SAGA_UNICODE

_saga_api_la_CPPFLAGS = $(PYTHON_CPPFLAGS) $(SWIGUNICODEFLAGS) $(DISABLELIBHARU)
_saga_api_la_LDFLAGS = -module -release $(VERSION) $(SWIG_LIB)
nodist__saga_api_la_SOURCES = saga_api_wrap.cxx
_saga_api_la_LIBADD = libsaga_api.la

pyexec_LTLIBRARIES = _saga_api.la
nodist_pyexec_PYTHON = saga_api.py

saga_api_wrap.cxx:
	$(SWIG) -includeall -python $(SWIGUNICODEFLAGS) $(DISABLELIBHARU) $(SAGAPYTHONFLAGS) ./saga_api.h 
endif

CLEANFILES = saga_api_wrap.cxx
