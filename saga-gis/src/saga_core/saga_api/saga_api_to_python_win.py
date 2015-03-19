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
		os.environ['WXWIN'] + '/lib/vc_x64_dll',
		os.environ['SAGA']
	],

	libraries = [
		'wxbase30u_xml',
		'wxbase30u',
		'saga_api'
	],

	extra_compile_args = [
		'-D__WXMSW__',
		'-DWXUSINGDLL',
		'-D_FILE_OFFSET_BITS=64',
		'-D_LARGE_FILES',
		'-D_LARGEFILE_SOURCE=1',
		'-DNO_GCC_PRAGMA',
		'-D_TYPEDEF_BYTE',
		'-D_TYPEDEF_WORD',
		'-D_SAGA_API_EXPORTS',
		'-DDEBUG',
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
