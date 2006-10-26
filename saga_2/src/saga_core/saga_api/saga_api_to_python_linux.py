from distutils.core import swig_saga_api, Extension

module1 = Extension(
	'_saga_api',

	sources = [
		'saga_api_wrap.cxx',
		'api_callback.cpp',
		'api_colors.cpp',
		'api_core.cpp',
		'api_file.cpp',
		'api_history.cpp',
		'api_memory.cpp',
		'api_string.cpp',
		'api_translator.cpp',
		'dataobject.cpp',
		'doc_html.cpp',
		'doc_pdf.cpp',
		'doc_svg.cpp',
		'geo_classes.cpp',
		'geo_functions.cpp',
		'grid.cpp',
		'grid_io.cpp',
		'grid_memory.cpp',
		'grid_operation.cpp',
		'grid_system.cpp',
		'mat_formula.cpp',
		'mat_grid_radius.cpp',
		'mat_indexing.cpp',
		'mat_matrix.cpp',
		'mat_regression.cpp',
		'mat_regression_multiple.cpp',
		'mat_spline.cpp',
		'mat_tools.cpp',
		'mat_trend.cpp',
		'module.cpp',
		'module_grid.cpp',
		'module_grid_interactive.cpp',
		'module_interactive.cpp',
		'module_interactive_base.cpp',
		'module_library.cpp',
		'module_library_interface.cpp',
		'parameter.cpp',
		'parameter_data.cpp', 
		'parameters.cpp',
		'saga_api.cpp',
		'shape.cpp',
		'shape_line.cpp',
		'shape_point.cpp',
		'shape_points.cpp',
		'shape_polygon.cpp',
		'shapes.cpp',
		'shapes_io.cpp',
		'shapes_search.cpp',
		'shapes_selection.cpp',
		'table.cpp',
		'table_dbase.cpp',
		'table_io.cpp',
		'table_record.cpp',
		'table_selection.cpp',
		'tin.cpp',
		'tin_elements.cpp',
		'tin_triangulation.cpp'
	],

	include_dirs = [
		'/usr/lib/wx/include/gtk2-ansi-release-2.6',
		'/usr/include/wx-2.6',
		'./'
	],

	libraries = [
		'pthread',
		'wx_gtk2_xrc-2.6',
		'wx_gtk2_html-2.6',
		'wx_gtk2_adv-2.6',
		'wx_gtk2_core-2.6',
		'wx_base_xml-2.6',
		'wx_base_net-2.6',
		'wx_base-2.6'
	],

	extra_compile_args = [
		'-g',
		'-fPIC',
		'-fpermissive',
		'-DGTK_NO_CHECK_CASTS',
		'-D__WXGTK__',
		'-D_FILE_OFFSET_BITS=64',
		'-D_LARGE_FILES',
		'-D_LARGEFILE_SOURCE=1',
		'-DNO_GCC_PRAGMA',
		'-D_SAGA_LINUX',
		'-D_TYPEDEF_BYTE',
		'-D_TYPEDEF_WORD',
		'-D_SAGA_API_EXPORTS'
	],

	extra_link_args = [
		'-g',
		'-fPIC',
		'-fpermissive',
		'-DGTK_NO_CHECK_CASTS',
		'-D__WXGTK__',
		'-D_FILE_OFFSET_BITS=64',
		'-D_LARGE_FILES',
		'-D_LARGEFILE_SOURCE=1',
		'-DNO_GCC_PRAGMA',
		'-D_SAGA_LINUX',
		'-D_TYPEDEF_BYTE',
		'-D_TYPEDEF_WORD',
		'-D_SAGA_API_EXPORTS'
	]
)

setup(
	name		= 'SAGA Python API',
	version		= '0.1',
	description	= '',
	ext_modules	= [module1]
)
