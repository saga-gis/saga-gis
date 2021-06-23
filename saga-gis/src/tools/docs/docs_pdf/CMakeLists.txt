option(HPDF_TOOLS "Build with Haru PDF dependent tools" OFF)

if(WITH_TOOLS AND HPDF_TOOLS)
	project(docs_pdf)

	include(../../CMakePluginTemplate.cmake)	# load common tool template

	find_package(HPDF REQUIRED)	# ubuntu package libhpdf-dev
	target_include_directories(${PROJECT_NAME} PRIVATE "${HPDF_INCLUDE_DIRS}")
	target_link_libraries(${PROJECT_NAME} "${HPDF_LIBRARIES}")
endif()
