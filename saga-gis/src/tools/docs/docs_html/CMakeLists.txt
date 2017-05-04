option(WITH_TOOL_DOCS_HTML "Check to install tool docs_html" OFF)

if (WITH_TOOL_DOCS_HTML OR WITH_ALL_STANDARD_TOOLS OR WITH_ALL_TOOLS)
	project(docs_html)

	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
