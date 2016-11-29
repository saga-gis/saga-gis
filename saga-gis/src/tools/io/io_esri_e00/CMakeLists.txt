option(WITH_TOOL_IO_ESRI_E00 "Check to install tool io_esri_e00" OFF)

if (WITH_TOOL_IO_ESRI_E00 OR WITH_ALL_TOOLS)
	project(io_esri_e00)

	if (NOT WITH_TOOL_IMAGERY_SVM_USE_SYSTEM_SVM)
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
	endif()
	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
