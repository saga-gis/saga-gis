option(WITH_TOOL_IMAGERY_VIGRA "Check to install imagery_vigra" OFF)
option(WITH_TOOL_IMAGERY_VIGRA_WITH_HDF5 "Check to use system version of libsvm instead of embedded copy" OFF)

if (WITH_TOOL_IMAGERY_VIGRA OR WITH_ALL_TOOLS)
	project(imagery_vigra)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )

	# link saga_api
	target_link_libraries(${PROJECT_NAME} saga_api)

	# link fftw3
	# on Ubuntu 16.04 package libfftw3-dev
	find_package(fftw3 REQUIRED)
	target_include_directories(${PROJECT_NAME} PRIVATE ${fftw3_INCLUDE_DIRS})
	target_link_libraries(${PROJECT_NAME} ${fftw3_LIBRARIES})
	# link vigraimpex
	# on Ubuntu 16.04 package libvigraimpex-dev
	find_package(vigra REQUIRED)
	target_include_directories(${PROJECT_NAME} PRIVATE ${vigra_INCLUDE_DIRS})
	target_link_libraries(${PROJECT_NAME} ${vigra_LIBRARIES})
	message("target_include_directories(${PROJECT_NAME} PRIVATE ${vigra_INCLUDE_DIRS}) ")
	message("target_link_libraries(${PROJECT_NAME} ${vigra_LIBRARIES}) ")

	if(WITH_TOOL_IMAGERY_VIGRA_WITH_HDF5)
		# link HDF5
		# on Ubuntu 16.04 package libhdf5-dev
		find_package(HDF5 REQUIRED)
		target_include_directories(${PROJECT_NAME} PRIVATE ${HDF5_INCLUDE_DIRS})
		target_link_libraries(${PROJECT_NAME} ${HDF5_LIBRARIES})
		message("target_include_directories(${PROJECT_NAME} PRIVATE ${HDF5_INCLUDE_DIRS}) ")
		message("target_link_libraries(${PROJECT_NAME} ${HDF5_LIBRARIES}) ")

		# add definition
		target_compile_definitions(${PROJECT_NAME} PUBLIC -DWITH_HDF5)

		#target_link_libraries(link_tester_${PROJECT_NAME} ${HDF5_LIBRARIES})
	endif()
endif()
