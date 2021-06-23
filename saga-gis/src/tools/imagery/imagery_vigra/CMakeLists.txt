option(VIGRA_TOOLS "Build with ViGra dependent tools" OFF)
option(VIGRA_WITH_HDF5 "Check for ViGra HDF5 support" OFF)

if(VIGRA_TOOLS)
	project(imagery_vigra)

	include(../../CMakePluginTemplate.cmake)	# load common tool template

#	target_link_libraries(${PROJECT_NAME} saga_api)	# link saga_api

	find_package(fftw3 REQUIRED)	# on Ubuntu package libfftw3-dev
	target_include_directories(${PROJECT_NAME} PRIVATE ${fftw3_INCLUDE_DIRS})
	target_link_libraries(${PROJECT_NAME} ${fftw3_LIBRARIES})

	find_package(vigra REQUIRED)	# on Ubuntu package libvigraimpex-dev
	target_include_directories(${PROJECT_NAME} PRIVATE ${VIGRA_INCLUDE_DIRS})
	target_link_libraries(${PROJECT_NAME} ${VIGRA_LIBRARIES})

	if(VIGRA_WITH_HDF5)
		find_package(HDF5 REQUIRED)	# on Ubuntu package libhdf5-dev
		target_include_directories(${PROJECT_NAME} PRIVATE ${HDF5_INCLUDE_DIRS})
		target_link_libraries(${PROJECT_NAME} ${HDF5_LIBRARIES})

		target_compile_definitions(${PROJECT_NAME} PUBLIC -DWITH_HDF5)	# add definition
	endif()
endif()
