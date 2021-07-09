# define headers
set(SAGA_API_PUBLIC_HEADERS
	api_core.h
	data_manager.h
	dataobject.h
	datetime.h
	geo_tools.h
	grid.h
	grids.h
	grid_pyramid.h
	mat_tools.h
	metadata.h
	parameters.h
	pointcloud.h
	saga_api.h
	shapes.h
	table_dbase.h
	table.h
	table_value.h
	tin.h
	tool.h
	tool_library.h
)

find_package(SWIG QUIET)
if(SWIG_FOUND)
	message(STATUS "SWIG for Python found")
	find_package(PythonLibs QUIET)
	if(PythonLibs_FOUND)
#		project(saga_api_python)

		message(STATUS "project: ${PROJECT_NAME}")

		message(STATUS "PythonLibs found")

		if(POLICY CMP0078)	# SWIG: use standard target name.
		  cmake_policy(SET CMP0078 NEW)
		endif()

		if(POLICY CMP0086)	# SWIG: use SWIG_MODULE_NAME property.
		  cmake_policy(SET CMP0086 NEW)
		endif()

		include(UseSWIG)

		include_directories("${PYTHON_INCLUDE_PATH}")
		include_directories("${PYTHON_INCLUDE_PATH}/../PC")	# msvc builds
		include_directories("..")

		set(CMAKE_SWIG_FLAGS "-D_SAGA_PYTHON -D_SWIG_WCHAR")

		set_property(SOURCE ../saga_api.i PROPERTY CPLUSPLUS ON)
		set_property(SOURCE ../saga_api.i PROPERTY COMPILE_OPTIONS "-includeall")

		swig_add_library(saga_api_python LANGUAGE Python SOURCES ../saga_api.i)

		set_property(TARGET saga_api_python PROPERTY OUTPUT_NAME saga_api)

		swig_link_libraries(saga_api_python saga_api)
		swig_link_libraries(saga_api_python ${PYTHON_LIBRARIES})

		if(MSVC)
			set(PYTHON_SITE_PACKAGES "${PYTHON_INCLUDE_PATH}/../Lib/site-packages")
		else() #if(NOT MSVC)
			execute_process(
				COMMAND python -c "import site; print(site.getsitepackages()[0])"
				OUTPUT_VARIABLE PYTHON_SITE_PACKAGES OUTPUT_STRIP_TRAILING_WHITESPACE
			)
		endif()

		if(EXISTS ${PYTHON_SITE_PACKAGES})
			message(STATUS "Python site-packages: ${PYTHON_SITE_PACKAGES}")
			install(TARGETS saga_api_python DESTINATION ${PYTHON_SITE_PACKAGES})
			install(FILES ${CMAKE_BINARY_DIR}/src/saga_core/saga_api/saga_api_python/saga_api.py DESTINATION ${PYTHON_SITE_PACKAGES})
			install(FILES ${CMAKE_SOURCE_DIR}/src/accessories/python/examples/saga_helper.py DESTINATION ${PYTHON_SITE_PACKAGES})
		endif()
	endif()
endif()
