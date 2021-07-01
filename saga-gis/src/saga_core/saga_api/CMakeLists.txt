project(saga_api)

message(STATUS "project: ${PROJECT_NAME}")

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

# define sources
set(SAGA_API_SOURCES
	api_callback.cpp
	api_colors.cpp
	api_core.cpp
	api_file.cpp
	api_memory.cpp
	api_string.cpp
	api_translator.cpp
	clipper.cpp
	data_manager.cpp
	dataobject.cpp
	datetime.cpp
	geo_classes.cpp
	geo_functions.cpp
	grid.cpp
	grid_io.cpp
	grid_memory.cpp
	grid_operation.cpp
	grid_pyramid.cpp
	grid_system.cpp
	grids.cpp
	kdtree.cpp
	mat_formula.cpp
	mat_grid_radius.cpp
	mat_indexing.cpp
	mat_matrix.cpp
	mat_regression.cpp
	mat_regression_multiple.cpp
	mat_regression_weighted.cpp
	mat_mRMR.cpp
	mat_spline.cpp
	mat_tools.cpp
	mat_trend.cpp
	metadata.cpp
	parameter.cpp
	parameter_data.cpp
	parameters.cpp
	pointcloud.cpp
	projections.cpp
	quadtree.cpp
	saga_api.cpp
	shape.cpp
	shape_line.cpp
	shape_part.cpp
	shape_point.cpp
	shape_points.cpp
	shape_polygon.cpp
	shapes.cpp
	shapes_io.cpp
	shapes_ogis.cpp
	shapes_polygons.cpp
	shapes_search.cpp
	shapes_selection.cpp
	table.cpp
	table_dbase.cpp
	table_io.cpp
	table_record.cpp
	table_selection.cpp
	tin.cpp
	tin_elements.cpp
	tin_triangulation.cpp
	tool.cpp
	tool_chain.cpp
	tool_grid.cpp
	tool_grid_interactive.cpp
	tool_interactive.cpp
	tool_interactive_base.cpp
	tool_library.cpp
	tool_library_interface.cpp
	tool_summary.cpp
	api_core.h
	clipper.hpp
	data_manager.h
	dataobject.h
	datetime.h
	geo_tools.h
	grid.h
	grid_pyramid.h
	mat_tools.h
	metadata.h
	nanoflann.hpp
	parameters.h
	pointcloud.h
	saga_api.h
	shapes.h
	table.h
	table_dbase.h
	table_value.h
	tin.h
	tool.h
	tool_chain.h
	tool_library.h
)

add_library(saga_api SHARED
	${SAGA_API_SOURCES}
	${SAGA_API_PUBLIC_HEADERS} # just cosmetic, show headers in IDE
)

target_include_directories(saga_api INTERFACE "${saga_core_include_dir}")

find_package(wxWidgets COMPONENTS base net xml html REQUIRED)
target_link_libraries(saga_api PRIVATE ${wxWidgets_LIBRARIES})

if(MSVC) # windows msvc
	target_compile_definitions(saga_api PUBLIC -D_SAGA_MSW -DUNICODE)
else() # unix like systems
	target_compile_definitions(saga_api PUBLIC -D_SAGA_LINUX)
	target_compile_definitions(saga_api PRIVATE -D${MTOOLSPATH} -D${MSHAREPATH})
	set_target_properties(saga_api PROPERTIES COMPILE_FLAGS -fPIC)
endif()

target_compile_definitions(saga_api PUBLIC -D_TYPEDEF_BYTE -D_TYPEDEF_WORD)
target_compile_definitions(saga_api PRIVATE -D_SAGA_API_EXPORTS)

# set version number for library
set_target_properties(saga_api PROPERTIES
	VERSION ${SAGA_VERSION_STRING}
	SOVERSION ${SAGA_VERSION_MAJOR}
)

# where to install the library
install(TARGETS saga_api DESTINATION lib)
install(FILES ${SAGA_API_PUBLIC_HEADERS} DESTINATION include/saga/saga_core/saga_api)

#-#-#-#-#-# SWIG / Python #-#-#-#-#-#
find_package(SWIG QUIET)
if(SWIG_FOUND)
	message(STATUS "SWIG for Python found")
	find_package(PythonLibs QUIET)
	if(PythonLibs_FOUND)
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
		include_directories(".")

		set(CMAKE_SWIG_FLAGS "-D_SAGA_PYTHON -D_SWIG_WCHAR")

		set_property(SOURCE saga_api.i PROPERTY CPLUSPLUS ON)
		set_property(SOURCE saga_api.i PROPERTY COMPILE_OPTIONS "-includeall")

		swig_add_library(_saga_api LANGUAGE Python SOURCES saga_api.i)

		swig_link_libraries(_saga_api saga_api)
		swig_link_libraries(_saga_api ${PYTHON_LIBRARIES})

		if(NOT MSVC)
			execute_process(COMMAND python -c "from distutils.sysconfig import get_python_lib; print get_python_lib()" OUTPUT_VARIABLE PYTHON_SITE_PACKAGES OUTPUT_STRIP_TRAILING_WHITESPACE)
			install(TARGETS _saga_api DESTINATION ${PYTHON_SITE_PACKAGES})
			install(FILES ${CMAKE_BINARY_DIR}/src/saga_core/saga_api/saga_api.py DESTINATION ${PYTHON_SITE_PACKAGES})
			install(FILES ${CMAKE_SOURCE_DIR}/src/saga_core/saga_api/saga_helper.py DESTINATION ${PYTHON_SITE_PACKAGES})
		endif()
	endif()
endif()
