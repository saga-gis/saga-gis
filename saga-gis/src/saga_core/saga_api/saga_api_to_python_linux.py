from distutils.core import setup, Extension

module1 = Extension(
	'_saga_api',

	sources = [
		'saga_api_wrap.cxx'
	],

	include_dirs = [
		'/usr/local/lib/wx/include/gtk2-ansi-release-2.8',
		'/usr/local/include/wx-2.8',
		'./'
	],

	libraries = [
		'pthread',
		'wx_base-2.8',
		'wx_gtk2_core-2.8',
		'saga_api',
#		'/opt/gnome/lib/pangox-1.0'
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
		'-D_SAGA_API_EXPORTS',
		'-D_SAGA_DONOTUSE_HARU',
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
		'-D_SAGA_DONOTUSE_HARU'
	]
)

setup(
	name		= 'SAGA Python API',
	version		= '0.1',
	description	= '',
	ext_modules	= [module1]
)
