option(WITH_TOOL_IMAGERY_SVM "Check to install tool imagery_svm" OFF)
option(WITH_TOOL_IMAGERY_SVM_USE_SYSTEM_SVM "Check to use system version of libsvm instead of embedded copy" OFF)

if (WITH_TOOL_IMAGERY_SVM OR WITH_ALL_TOOLS)
	project(imagery_svm)

	if (NOT WITH_TOOL_IMAGERY_SVM_USE_SYSTEM_SVM)
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

	if (WITH_TOOL_IMAGERY_SVM_USE_SYSTEM_SVM)
		# load libsvm
		# ubuntu 16.04 packages libsvm-dev
		find_package(libsvm REQUIRED)
		target_include_directories(${PROJECT_NAME} PRIVATE ${libsvm_INCLUDE_DIRS})
		target_link_libraries(${PROJECT_NAME} ${libsvm_LIBRARIES})
	endif()
endif()
