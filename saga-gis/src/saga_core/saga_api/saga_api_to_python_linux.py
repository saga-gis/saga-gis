#!/usr/bin/env python

# SAGA Python Interface distutils setup script
# called by saga_api_to_python.sh script

from distutils.core import setup, Extension

module1 = Extension(
	'_saga_api',

	sources = [
		'saga_api_wrap.cxx'
	],

	include_dirs = [
		'./'
	],

	libraries = [
		'pthread',
		'saga_api'
	],

	extra_compile_args = [
		'-g',
		'-fPIC',
		'-fpermissive',
		'-fopenmp',
		'-DGTK_NO_CHECK_CASTS',
		'-D_FILE_OFFSET_BITS=64',
		'-D_LARGE_FILES',
		'-D_LARGEFILE_SOURCE=1',
		'-DNO_GCC_PRAGMA',
		'-D_SAGA_LINUX',
		'-D_TYPEDEF_BYTE',
		'-D_TYPEDEF_WORD',
		'-D_SAGA_API_EXPORTS',
		'-D_SAGA_DONOTUSE_HARU',
		'-D_SAGA_PYTHON'
	],

	extra_link_args = [
		'-g',
		'-fPIC',
		'-fpermissive',
		'-fopenmp',
		'-lgomp',
		'-DGTK_NO_CHECK_CASTS',
		'-D_FILE_OFFSET_BITS=64',
		'-D_LARGE_FILES',
		'-D_LARGEFILE_SOURCE=1',
		'-DNO_GCC_PRAGMA',
		'-D_SAGA_LINUX',
		'-D_TYPEDEF_BYTE',
		'-D_TYPEDEF_WORD',
		'-D_SAGA_API_EXPORTS'
		'-D_SAGA_DONOTUSE_HARU',
		'-D_SAGA_PYTHON'
	]
)

setup(
	name		= 'SAGA Python API',
	version		= '1.0',
	description	= '',
	ext_modules	= [module1]
)
