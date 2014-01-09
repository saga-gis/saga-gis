#!/usr/bin/env python

# SAGA Python Interface distutils setup script
# called by saga_api_to_python.sh script
# adjust the paths to your gtk/wx install

from distutils.core import setup, Extension

include_gtk = '/usr/lib/wx/include/gtk2-unicode-3.0'
include_wx = '/usr/include/wx-3.0'

module1 = Extension(
	'_saga_api',

	sources = [
		'saga_api_wrap.cxx'
	],

	include_dirs = [
		include_gtk,
		include_wx,
		'./'
	],

	libraries = [
		'pthread',
		'wx_baseu-3.0',
		'wx_gtk2u_core-3.0',
		'saga_api'
	],

	extra_compile_args = [
		'-g',
		'-fPIC',
		'-fpermissive',
		'-fopenmp',
		'-DGTK_NO_CHECK_CASTS',
		'-D__WXGTK__',
		'-D_FILE_OFFSET_BITS=64',
		'-D_LARGE_FILES',
		'-D_LARGEFILE_SOURCE=1',
		'-DNO_GCC_PRAGMA',
		'-D_SAGA_LINUX',
		'-D_TYPEDEF_BYTE',
		'-D_TYPEDEF_WORD',
		'-D_SAGA_API_EXPORTS',
		'-D_SAGA_DONOTUSE_HARU',
		'-D_SAGA_PYTHON',
		'-D_SAGA_UNICODE'
	],

	extra_link_args = [
		'-g',
		'-fPIC',
		'-fpermissive',
		'-fopenmp',
		'-lgomp',
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
		'-D_SAGA_DONOTUSE_HARU',
		'-D_SAGA_PYTHON',
		'-D_SAGA_UNICODE'
	]
)

setup(
	name		= 'SAGA Python API',
	version		= '0.2',
	description	= '',
	ext_modules	= [module1]
)
