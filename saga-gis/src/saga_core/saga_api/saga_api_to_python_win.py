from distutils.core import setup, Extension, os

module1 = Extension(
	'_saga_api',

	sources = [
		'saga_api_wrap.cxx'
	],

	include_dirs = [
		os.environ['WXWIN'] + '/include',
		os.environ['WXWIN'] + '/include/msvc',
		'./'
	],

	library_dirs = [
		os.environ['WXWINLIB'],
		os.environ['SAGA_LIB']
	],

	libraries = [
		'wxbase31u_xml',
		'wxbase31u',
		'saga_api'
	],

	extra_compile_args = [
		'-D__WXMSW__',
		'-DWXUSINGDLL',
		'-D_FILE_OFFSET_BITS=64',
		'-D_LARGE_FILES',
		'-D_LARGEFILE_SOURCE=1',
		'-D_TYPEDEF_BYTE',
		'-D_TYPEDEF_WORD',
		'-D_SAGA_API_EXPORTS',
		'-D_SAGA_UNICODE',
		'-D_SAGA_PYTHON'
	]
)

setup(
	name 		= 'SAGA Python API',
	version 	= '0.1',
	description = '',
	ext_modules = [module1]
)
