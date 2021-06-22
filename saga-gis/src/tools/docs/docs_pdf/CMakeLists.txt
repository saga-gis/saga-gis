option(WITH_TOOLS_HARU "Build with HARU PDF dependent tools" OFF)

if(WITH_TOOLS AND WITH_TOOLS_HARU)
	project(docs_pdf)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )

	# load Haru PDF library
	# ubuntu 16.04 package libhpdf-dev
	find_package(HPDF REQUIRED)
	target_include_directories(${PROJECT_NAME} PRIVATE "${HPDF_INCLUDE_DIRS}")
	target_link_libraries(${PROJECT_NAME} "${HPDF_LIBRARIES}")
endif()
