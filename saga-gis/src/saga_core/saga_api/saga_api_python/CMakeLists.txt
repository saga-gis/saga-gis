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
	find_package(Python COMPONENTS Interpreter Development)
	if(Python_FOUND)
#		project(saga_api_python)

		message(STATUS "project: ${PROJECT_NAME}")

		message(STATUS "Python found")

		if(POLICY CMP0078)	# SWIG: use standard target name.
			cmake_policy(SET CMP0078 NEW)
		endif()

		if(POLICY CMP0086)	# SWIG: use SWIG_MODULE_NAME property.
			cmake_policy(SET CMP0086 NEW)
		endif()

		include(UseSWIG)

		include_directories("${Python_INCLUDE_DIRS}")
		include_directories("${Python_INCLUDE_DIRS}/../PC")	# msvc builds
		include_directories("..")

		set(CMAKE_SWIG_FLAGS "-D_SAGA_PYTHON -D_SWIG_WCHAR")

		set_property(SOURCE ../saga_api.i PROPERTY CPLUSPLUS ON)
		set_property(SOURCE ../saga_api.i PROPERTY COMPILE_OPTIONS "-includeall")

		swig_add_library(saga_api_python LANGUAGE Python SOURCES ../saga_api.i)

		set_property(TARGET saga_api_python PROPERTY OUTPUT_NAME saga_api)

		swig_link_libraries(saga_api_python saga_api)
		swig_link_libraries(saga_api_python ${Python_LIBRARIES})


		if(EXISTS ${Python_SITELIB})
			option(WITH_PYTHON_PKG "PySAGA package installation (if not checked API modules become installed directly in Python's site/dist-packages folder)" ON)
			if(WITH_PYTHON_PKG)
				message(STATUS "SAGA-Python-API installed to: ${Python_SITELIB}/PySAGA")
				install(TARGETS saga_api_python                                                      DESTINATION ${Python_SITELIB}/PySAGA)
				install(FILES ${CMAKE_BINARY_DIR}/src/saga_core/saga_api/saga_api_python/saga_api.py DESTINATION ${Python_SITELIB}/PySAGA)
				install(FILES ${CMAKE_SOURCE_DIR}/src/accessories/python/__init__.py                 DESTINATION ${Python_SITELIB}/PySAGA)
				install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/accessories/python/data                    DESTINATION ${Python_SITELIB}/PySAGA FILES_MATCHING PATTERN "*.py")
			else()
				message(STATUS "SAGA-Python-API installed to: ${Python_SITELIB}")
				install(TARGETS saga_api_python                                                      DESTINATION ${Python_SITELIB})
				install(FILES ${CMAKE_BINARY_DIR}/src/saga_core/saga_api/saga_api_python/saga_api.py DESTINATION ${Python_SITELIB})
			endif()
		endif()
	endif()
endif()
