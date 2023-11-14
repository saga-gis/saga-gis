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

		swig_link_libraries(saga_api_python saga_api)
		swig_link_libraries(saga_api_python ${Python_LIBRARIES})

		if(MSVC)
			set_property(TARGET saga_api_python PROPERTY OUTPUT_NAME saga_api_python)

			string(REPLACE "/" "\\" PySAGA_TARGET "\"${CMAKE_BINARY_DIR}/saga_x64/PySAGA\"")
			string(REPLACE "/" "\\" PySAGA_SOURCE "\"${CMAKE_SOURCE_DIR}/src/accessories/python\"")
			string(REPLACE "/" "\\" PySAGA_Py_API "\"${CMAKE_BINARY_DIR}/src/saga_core/saga_api/saga_api_python/saga_api.py\"")
			string(REPLACE "/" "\\" PySAGA_Py_BIN "\"${CMAKE_BINARY_DIR}/src/saga_core/saga_api/saga_api_python/Release/*.pyd\"")
			message(STATUS "SAGA-Python-API installs to: ${PySAGA_TARGET}")
			add_custom_command(TARGET saga_api_python POST_BUILD
				COMMAND IF EXIST "${PySAGA_TARGET}" RMDIR /S/Q "${PySAGA_TARGET}"
				COMMAND MKDIR "${PySAGA_TARGET}"
				COMMAND XCOPY /C/S/Q/Y/H "${PySAGA_SOURCE}" "${PySAGA_TARGET}"
				COMMAND COPY "${PySAGA_Py_API}" "${PySAGA_TARGET}"
				COMMAND COPY "${PySAGA_Py_BIN}" "${PySAGA_TARGET}"
			)
		else()
			set_property(TARGET saga_api_python PROPERTY OUTPUT_NAME saga_api)
			# check if non-standard installation is performed
			
			if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
				set(PYTHON_INSTALL_DIR ${Python_SITEARCH})
			else()
				set(PYTHON_INSTALL_DIR ${CMAKE_INSTALL_PREFIX})
			endif()
			
			option(WITH_PYTHON_PKG "PySAGA package installation (if not checked API modules become installed directly in Python's site/dist-packages folder)" ON)
			if(WITH_PYTHON_PKG)
				message(STATUS "SAGA-Python-API installs to: ${PYTHON_INSTALL_DIR}/PySAGA")
				install(TARGETS saga_api_python                                                      DESTINATION ${PYTHON_INSTALL_DIR}/PySAGA)
				install(FILES ${CMAKE_BINARY_DIR}/src/saga_core/saga_api/saga_api_python/saga_api.py DESTINATION ${PYTHON_INSTALL_DIR}/PySAGA)
				install(FILES ${CMAKE_SOURCE_DIR}/src/accessories/python/__init__.py                 DESTINATION ${PYTHON_INSTALL_DIR}/PySAGA)
				install(FILES ${CMAKE_SOURCE_DIR}/src/accessories/python/helper.py                   DESTINATION ${PYTHON_INSTALL_DIR}/PySAGA)
				install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/accessories/python/data                    DESTINATION ${PYTHON_INSTALL_DIR}/PySAGA FILES_MATCHING PATTERN "*.py")
				if(EXISTS ${CMAKE_SOURCE_DIR}/src/accessories/python/tools)
					install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/accessories/python/tools               DESTINATION ${PYTHON_INSTALL_DIR}/PySAGA FILES_MATCHING PATTERN "*.py")
				endif()
			else()
				message(STATUS "SAGA-Python-API installed to: ${PYTHON_INSTALL_DIR}")
				install(TARGETS saga_api_python                                                      DESTINATION ${PYTHON_INSTALL_DIR})
				install(FILES ${CMAKE_BINARY_DIR}/src/saga_core/saga_api/saga_api_python/saga_api.py DESTINATION ${PYTHON_INSTALL_DIR})
			endif()
		endif()
	endif()
endif()
