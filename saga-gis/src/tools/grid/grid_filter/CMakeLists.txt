option(WITH_TOOL_GRID_FILTER "Check to install tool grid_filter" OFF)

if (WITH_TOOL_GRID_FILTER OR WITH_ALL_TOOLS)
	project(grid_filter)

	# add files in sudirectories
	set(SAGA_TOOL_CUSTOM_SOURCE_LIST
		geodesic_morph_rec/bin_geovinc.c
		geodesic_morph_rec/combcontour.c
		geodesic_morph_rec/geovinc.c
		geodesic_morph_rec/storeorg.c
	)
	set(SAGA_TOOL_CUSTOM_HEADER_LIST
		geodesic_morph_rec/bin_geovinc.h
		geodesic_morph_rec/combcontour.h
		geodesic_morph_rec/geovinc.h
		geodesic_morph_rec/listfuncs.inc
		geodesic_morph_rec/storeorg.h
	)
	# load common tool template
	include(../../CMakePluginTemplate.cmake )
endif()
