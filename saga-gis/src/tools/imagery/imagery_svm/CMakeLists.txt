option(WITH_SYSTEM_SVM "Check to use system version of libsvm instead of embedded copy" OFF)

if(WITH_TOOLS)
	project(imagery_svm)

	if(WITH_SYSTEM_SVM)
		# load libsvm
		# ubuntu 16.04 packages libsvm-dev
		find_package(libsvm REQUIRED)
		target_include_directories(${PROJECT_NAME} PRIVATE ${libsvm_INCLUDE_DIRS})
		target_link_libraries(${PROJECT_NAME} ${libsvm_LIBRARIES})
	else()
		# add files in sudirectories
		set(SAGA_TOOL_CUSTOM_SOURCE_LIST
			svm/svm.cpp
		)
		set(SAGA_TOOL_CUSTOM_HEADER_LIST
			svm/svm.h
		)
	endif()

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
