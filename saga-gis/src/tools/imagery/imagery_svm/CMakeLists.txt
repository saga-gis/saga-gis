option(WITH_SYSTEM_SVM "Check to use system version of libsvm instead of embedded copy" OFF)

if(WITH_TOOLS)
	project(imagery_svm)

	find_package(svm QUIET)	# ubuntu packages libsvm-dev
	if(WITH_SYSTEM_SVM AND SVM_FOUND)
		target_include_directories(${PROJECT_NAME} PRIVATE ${SVM_INCLUDE})
		target_link_libraries(${PROJECT_NAME} ${SVM_LIBRARY})
	else()
		set(SAGA_TOOL_CUSTOM_SOURCE_LIST svm/svm.cpp)
		set(SAGA_TOOL_CUSTOM_HEADER_LIST svm/svm.h)
	endif()

	include(../../CMakePluginTemplate.cmake)	# load common tool template
endif()
