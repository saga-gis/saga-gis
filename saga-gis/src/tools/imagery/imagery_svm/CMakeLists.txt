option(WITH_SYSTEM_SVM "Check to use system version of libsvm instead of embedded copy" OFF)

if(WITH_TOOLS)
	project(imagery_svm)

	if(WITH_SYSTEM_SVM)
		find_package(SVM QUIET)	# ubuntu packages libsvm-dev
	endif()

	if(NOT SVM_FOUND)
		set(SAGA_TOOL_CUSTOM_SOURCE_LIST ./svm/svm.cpp)
		set(SAGA_TOOL_CUSTOM_HEADER_LIST ./svm/svm.h)
	endif()

	include(../../CMakePluginTemplate.cmake)	# load common tool template
	
	if(SVM_FOUND)
		target_include_directories(${PROJECT_NAME} PRIVATE ${SVM_INCLUDE})
		target_link_libraries     (${PROJECT_NAME}         ${SVM_LIBRARY})
	endif()
endif()
