option(WITH_SYSTEM_DXF "Check to use system version of dxflib library instead of embedded copy" OFF)

if(WITH_TOOLS)
	project(io_shapes_dxf)

	if(WITH_SYSTEM_DXF)
		# link dxflib library
		# Ubuntu 16.04 package libdxflib-dev
		find_package(dxflib REQUIRED)
		target_include_directories(${PROJECT_NAME} PRIVATE ${dxflib_INCLUDE_DIRS})
		target_link_libraries(${PROJECT_NAME} ${dxflib_LIBRARIES})
	else()
		# add files in sudirectories
		set(SAGA_TOOL_CUSTOM_SOURCE_LIST
			dxflib/dl_dxf.cpp
			dxflib/dl_writer_ascii.cpp
		)
		set(SAGA_TOOL_CUSTOM_HEADER_LIST
			dxflib/dl_attributes.h
			dxflib/dl_codes.h
			dxflib/dl_creationadapter.h
			dxflib/dl_creationinterface.h
			dxflib/dl_dxf.h
			dxflib/dl_entities.h
			dxflib/dl_exception.h
			dxflib/dl_extrusion.h
			dxflib/dl_writer.h
			dxflib/dl_writer_ascii.h
		)
	endif()

	# load common tool template
	include(../../CMakePluginTemplate.cmake)
endif()
