if(WITH_TOOLS)
	find_package(HPDF)	# ubuntu package libhpdf-dev
	if(HPDF_FOUND)
		project(docs_pdf)
		include(../../CMakePluginTemplate.cmake)	# load common tool template
		target_include_directories(${PROJECT_NAME} PRIVATE "${HPDF_INCLUDE_DIRS}")
		target_link_libraries(${PROJECT_NAME} "${HPDF_LIBRARIES}")
	endif()
endif()
