option(WITH_TOOL_GRID_CALCULUS "Check to install tool grid_calculus" OFF)

if (WITH_TOOL_GRID_CALCULUS OR WITH_ALL_STANDARD_TOOLS OR WITH_ALL_TOOLS)
	project(grid_calculus)

	# add files in sudirectories
	set(SAGA_TOOL_CUSTOM_SOURCE_LIST
		../grid_filter/geodesic_morph_rec/errfunc.c
		../grid_filter/geodesic_morph_rec/spezfunc.c
		../grid_filter/geodesic_morph_rec/storeorg.c
	)
	set(SAGA_TOOL_CUSTOM_HEADER_LIST
		../grid_filter/geodesic_morph_rec/errfunc.h
		../grid_filter/geodesic_morph_rec/rho.h
		../grid_filter/geodesic_morph_rec/spezfunc.h
		../grid_filter/geodesic_morph_rec/storeorg.h
	)
	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
