option(WITH_TOOL_DOCS_PDF "Check to install tool docs_pdf" OFF)

if (WITH_TOOL_DOCS_PDF OR WITH_ALL_TOOLS)
	project(docs_pdf)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )

	# load Haru PDF library
	# ubuntu 16.04 package libhpdf-dev
	find_package(HPDF REQUIRED)
	target_include_directories(${PROJECT_NAME} PRIVATE "${HPDF_INCLUDE_DIRS}")
	target_link_libraries(${PROJECT_NAME} "${HPDF_LIBRARIES}")
endif()
