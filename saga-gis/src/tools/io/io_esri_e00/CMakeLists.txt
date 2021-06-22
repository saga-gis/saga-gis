if(WITH_TOOLS)
	project(io_esri_e00)

	# add files in sudirectories
	set(SAGA_TOOL_CUSTOM_SOURCE_LIST
		e00compr/cpl_conv.c
		e00compr/cpl_error.c
		e00compr/cpl_vsisimple.c
		e00compr/e00read.c
		e00compr/e00write.c
	)
	set(SAGA_TOOL_CUSTOM_HEADER_LIST
		e00compr/cpl_conv.h
		e00compr/cpl_error.h
		e00compr/cpl_port.h
		e00compr/cpl_vsi.h
		e00compr/e00compr.h
	)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
