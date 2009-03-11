#
# $Id: Makefile.am,v 1.12 2009-03-11 15:08:35 oconrad Exp $
#
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
if SAGA_UNICODE
AM_CXXFLAGS = -fPIC -Wall `wx-config --unicode=yes --cxxflags` -D_SAGA_LINUX -D_SAGA_UNICODE -D_TYPEDEF_BYTE -D_TYPEDEF_WORD -D_SAGA_API_EXPORTS -D_SAGA_DONOTUSE_HARU $(DBGFLAGS)
AM_LDFLAGS = -fPIC -shared -Wall `wx-config --unicode=yes --libs`
else
AM_CXXFLAGS = -fPIC -Wall `wx-config --unicode=no --cxxflags` -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD -D_SAGA_API_EXPORTS -D_SAGA_DONOTUSE_HARU $(DBGFLAGS)
AM_LDFLAGS = -fPIC -shared -Wall `wx-config --unicode=no --libs`
endif

lib_LTLIBRARIES = libsaga_api.la
libsaga_api_la_SOURCES =\
api_callback.cpp\
api_colors.cpp\
api_core.cpp\
api_file.cpp\
api_history.cpp\
api_memory.cpp\
api_string.cpp\
api_translator.cpp\
dataobject.cpp\
doc_html.cpp\
doc_pdf.cpp\
doc_svg.cpp\
geo_classes.cpp\
geo_functions.cpp\
grid.cpp\
grid_io.cpp\
grid_memory.cpp\
grid_operation.cpp\
grid_system.cpp\
mat_formula.cpp\
mat_grid_radius.cpp\
mat_indexing.cpp\
mat_matrix.cpp\
mat_regression.cpp\
mat_regression_multiple.cpp\
mat_spline.cpp\
mat_tools.cpp\
mat_trend.cpp\
module.cpp\
module_grid.cpp\
module_grid_interactive.cpp\
module_interactive.cpp\
module_interactive_base.cpp\
module_library.cpp\
module_library_interface.cpp\
parameter.cpp\
parameter_data.cpp\
parameters.cpp\
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
api_core.h\
dataobject.h\
doc_html.h\
doc_pdf.h\
doc_svg.h\
geo_tools.h\
grid.h\
mat_tools.h\
module.h\
module_library.h\
parameters.h\
saga_api.h\
shapes.h\
table.h\
table_dbase.h\
table_value.h\
tin.h

install-exec-hook:
	-ldconfig
