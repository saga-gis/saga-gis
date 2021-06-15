project(saga_gdi)
message(STATUS "project: ${PROJECT_NAME}")

# define sources
set(SAGA_GDI_SOURCES
	3d_view_canvas.cpp
	3d_view_dialog.cpp
	3d_view_panel.cpp
	3d_view_projector.cpp
	sgdi_controls.cpp
	sgdi_diagram.cpp
	sgdi_dialog.cpp
	sgdi_helper.cpp
	sgdi_layout_items.cpp
	saga_gdi.cpp
	3d_view.h
	3d_view_tools.h
	saga_gdi.h
	sgdi_controls.h
	sgdi_core.h
	sgdi_diagram.h
	sgdi_dialog.h
	sgdi_helper.h
	sgdi_layout_items.h
)
set(SAGA_GDI_PUBLIC_HEADERS
	3d_view.h
	3d_view_tools.h
	saga_gdi.h
	sgdi_controls.h
	sgdi_core.h
	sgdi_diagram.h
	sgdi_dialog.h
	sgdi_helper.h
	sgdi_layout_items.h
)

add_library(saga_gdi SHARED
	${SAGA_GDI_SOURCES}
	${SAGA_GDI_PUBLIC_HEADERS} # just cosmetic, show headers in IDE
)
# link saga_api
target_link_libraries(saga_gdi saga_api)

# define for compiling libraries
target_compile_definitions(${PROJECT_NAME} PRIVATE -D_SAGA_GDI_EXPORTS)

# include directories of wxWidgets are already present
find_package(wxWidgets REQUIRED)
target_link_libraries(saga_gdi ${wxWidgets_LIBRARIES})

# set version number for library
set_target_properties(saga_gdi PROPERTIES
	VERSION ${SAGA_VERSION_STRING}
	SOVERSION ${SAGA_VERSION_MAJOR})

# where to install the binary
install(TARGETS saga_gdi DESTINATION lib)
install(FILES ${SAGA_GDI_PUBLIC_HEADERS} DESTINATION include/saga/saga_core/saga_gdi)
