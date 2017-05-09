message("folder: tools/statistics")

add_subdirectory(statistics_points)
add_subdirectory(statistics_grid)
add_subdirectory(statistics_regression)

if(WITH_GDI)
	add_subdirectory(statistics_kriging)
endif()
