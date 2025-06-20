project(saga_gui)
message(STATUS "project: ${PROJECT_NAME}")

# define sources
set(SAGA_GUI_SOURCES
	active.cpp
	active_attributes.cpp
	active_attributes_control.cpp
	active_description.cpp
	active_history.cpp
	active_info.cpp
	active_legend.cpp
	active_parameters.cpp
	callback.cpp
	data_source.cpp
	data_source_files.cpp
	data_source_odbc.cpp
	data_source_pgsql.cpp
	dlg_about.cpp
	dlg_about_logo.cpp
	dlg_base.cpp
	dlg_choices.cpp
	dlg_colors.cpp
	dlg_colors_control.cpp
	dlg_list.cpp
	dlg_parameters.cpp
	dlg_table.cpp
	dlg_table_control.cpp
	dlg_text.cpp
	helper.cpp
	info.cpp
	info_messages.cpp
	parameters_control.cpp
	parameters_properties.cpp
	project.cpp
	res_commands.cpp
	res_controls.cpp
	res_dialogs.cpp
	res_images.cpp
	saga.cpp
	saga_frame.cpp
	saga_frame_droptarget.cpp
	view_base.cpp
	view_histogram.cpp
	view_layout.cpp
	view_layout_control.cpp
	view_layout_info.cpp
	view_map.cpp
	view_map_3d.cpp
	view_map_3d_panel.cpp
	view_map_control.cpp
	view_ruler.cpp
	view_scatterplot.cpp
	view_table.cpp
	view_table_control.cpp
	view_table_data.cpp
	view_table_diagram.cpp
	wksp.cpp
	wksp_base_control.cpp
	wksp_base_item.cpp
	wksp_base_manager.cpp
	wksp_data_control.cpp
	wksp_data_item.cpp
	wksp_data_layers.cpp
	wksp_data_manager.cpp
	wksp_data_menu_file.cpp
	wksp_data_menu_files.cpp
	wksp_grid.cpp
	wksp_grid_manager.cpp
	wksp_grid_system.cpp
	wksp_grids.cpp
	wksp_layer.cpp
	wksp_layer_classify.cpp
	wksp_layer_legend.cpp
	wksp_map.cpp
	wksp_map_basemap.cpp
	wksp_map_buttons.cpp
	wksp_map_control.cpp
	wksp_map_graticule.cpp
	wksp_map_layer.cpp
	wksp_map_manager.cpp
	wksp_pointcloud.cpp
	wksp_pointcloud_manager.cpp
	wksp_shapes.cpp
	wksp_shapes_edit.cpp
	wksp_shapes_line.cpp
	wksp_shapes_manager.cpp
	wksp_shapes_point.cpp
	wksp_shapes_points.cpp
	wksp_shapes_polygon.cpp
	wksp_shapes_type.cpp
	wksp_table.cpp
	wksp_table_manager.cpp
	wksp_tin.cpp
	wksp_tin_manager.cpp
	wksp_tool.cpp
	wksp_tool_control.cpp
	wksp_tool_library.cpp
	wksp_tool_manager.cpp
	wksp_tool_menu.cpp
	active.h
	active_attributes.h
	active_attributes_control.h
	active_description.h
	active_history.h
	active_info.h
	active_legend.h
	active_parameters.h
	callback.h
	data_source.h
	data_source_files.h
	data_source_odbc.h
	data_source_pgsql.h
	dlg_about.h
	dlg_about_logo.h
	dlg_base.h
	dlg_choices.h
	dlg_colors.h
	dlg_colors_control.h
	dlg_list.h
	dlg_parameters.h
	dlg_table.h
	dlg_table_control.h
	dlg_text.h
	helper.h
	info.h
	info_messages.h
	parameters_control.h
	parameters_properties.h
	project.h
	res_commands.h
	res_controls.h
	res_dialogs.h
	res_images.h
	saga.h
	saga_frame.h
	saga_frame_droptarget.h
	view_base.h
	view_histogram.h
	view_layout.h
	view_layout_control.h
	view_layout_info.h
	view_map.h
	view_map_3d.h
	view_map_3d_panel.h
	view_map_control.h
	view_ruler.h
	view_scatterplot.h
	view_table.h
	view_table_control.h
	view_table_data.h
	view_table_diagram.h
	wksp.h
	wksp_base_control.h
	wksp_base_item.h
	wksp_base_manager.h
	wksp_data_control.h
	wksp_data_item.h
	wksp_data_layers.h
	wksp_data_manager.h
	wksp_data_menu_file.h
	wksp_data_menu_files.h
	wksp_grid.h
	wksp_grid_manager.h
	wksp_grid_system.h
	wksp_grids.h
	wksp_layer.h
	wksp_layer_classify.h
	wksp_layer_legend.h
	wksp_map.h
	wksp_map_basemap.h
	wksp_map_buttons.h
	wksp_map_control.h
	wksp_map_graticule.h
	wksp_map_layer.h
	wksp_map_manager.h
	wksp_pointcloud.h
	wksp_pointcloud_manager.h
	wksp_shapes.h
	wksp_shapes_line.h
	wksp_shapes_manager.h
	wksp_shapes_point.h
	wksp_shapes_points.h
	wksp_shapes_polygon.h
	wksp_shapes_type.h
	wksp_table.h
	wksp_table_manager.h
	wksp_tin.h
	wksp_tin_manager.h
	wksp_tool.h
	wksp_tool_control.h
	wksp_tool_library.h
	wksp_tool_manager.h
	wksp_tool_menu.h
)

add_executable(saga_gui ${SAGA_GUI_SOURCES} res/saga_gui.rc)

# link needed saga libraries
target_link_libraries(saga_gui saga_api saga_gdi)

# find and link wxWidgets libraries
find_package(wxWidgets COMPONENTS adv aui base core html net propgrid xml REQUIRED QUIET)
target_link_libraries(saga_gui ${wxWidgets_LIBRARIES})

# add compile definition needed
if(MSVC)
	option(WITH_GUI_TABBED_MDI "GUI with Tabbed MDI" OFF)
	if(WITH_GUI_TABBED_MDI)
		target_compile_definitions(saga_gui PRIVATE MDI_TABBED)
	endif()

	target_compile_definitions(saga_gui PUBLIC -D_SAGA_MSW -DUNICODE)
	target_link_options(saga_gui PRIVATE /SUBSYSTEM:WINDOWS)
	
	option(WITH_DPI_AWARENESS "GUI as DPI aware application" ON)
	if(WITH_DPI_AWARENESS)
		message(STATUS "GUI as DPI aware application")
		target_compile_definitions(saga_gui PRIVATE DPI_AWARE)
		set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
		set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /MANIFEST:NO")
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /MANIFEST:NO")
	else()
		message(STATUS "GUI without DPI awareness")
	endif()
	
else() # unix like systems
	set_target_properties(saga_gui PROPERTIES COMPILE_FLAGS -fPIC)
	target_compile_definitions(saga_gui PUBLIC -D_SAGA_LINUX)
	target_compile_definitions(saga_gui PRIVATE -D${MTOOLSPATH} -D${MSHAREPATH})
endif()

find_package(Git)
if(GIT_FOUND)
	IF(EXISTS "${CMAKE_SOURCE_DIR}/../.git")
		execute_process(
			COMMAND git log -1 --format=%h
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			OUTPUT_VARIABLE GIT_HASH
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)
		message("git hash: ${GIT_HASH} to ${PROJECT_SOURCE_DIR}/git_hash.h")
	else()
		message("not a git repository, git hash not found")
	endif()
else()
	message("git executable not found")
endif(GIT_FOUND)
configure_file("${PROJECT_SOURCE_DIR}/git_hash.h.in" "${PROJECT_SOURCE_DIR}/git_hash.h" @ONLY)

set(COMPILER "${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
add_definitions(-DCOMPILER="${COMPILER}")

# add man page
add_subdirectory(man)
# add res directory
add_subdirectory(res)

# where to install the binary
install(TARGETS saga_gui DESTINATION bin)

if(MSVC)
	string(REPLACE "/" "\\" SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/")
	add_custom_command(TARGET saga_gui POST_BUILD
		COMMAND COPY "${SOURCE_DIR}res\\saga.rus.txt" "$(OutDir)saga.rus.txt"
		COMMAND COPY "${SOURCE_DIR}res\\saga.ger.txt" "$(OutDir)saga.ger.txt"
		COMMAND COPY "${SOURCE_DIR}res\\saga.lng.txt" "$(OutDir)saga.lng.txt"
		COMMAND COPY "${SOURCE_DIR}res\\saga.bra.txt" "$(OutDir)saga.bra.txt"
		COMMAND COPY "${SOURCE_DIR}res\\saga.ger.txt" "$(OutDir)saga.ger.txt"
		COMMAND COPY "${SOURCE_DIR}res\\saga.rus.txt" "$(OutDir)saga.rus.txt"
		COMMAND COPY "${SOURCE_DIR}res\\saga_tip.txt" "$(OutDir)saga_gui.tip"
		COMMAND COPY "${SOURCE_DIR}res\\saga_srs.txt" "$(OutDir)saga.srs"
		COMMAND COPY "${SOURCE_DIR}res\\saga_srs_preferences.txt" "$(OutDir)saga_preferences.srs"
		COMMAND IF NOT EXIST "$(OutDir)tools" MKDIR "$(OutDir)tools"
		COMMAND IF EXIST "$(OutDir)tools\\toolchains" RMDIR /S/Q "$(OutDir)tools\\toolchains"
		COMMAND MKDIR    "$(OutDir)tools\\toolchains"
		COMMAND XCOPY /C/Q/Y/H "${SOURCE_DIR}..\\..\\accessories\\toolchains\\*.xml" "$(OutDir)tools\\toolchains"
		COMMAND IF EXIST "$(OutDir)resources" RMDIR /S/Q "$(OutDir)resources"
		COMMAND MKDIR    "$(OutDir)resources"
		COMMAND XCOPY /S/E/C/Q/Y/H "${SOURCE_DIR}..\\..\\accessories\\resources" "$(OutDir)resources"
	)
endif()
