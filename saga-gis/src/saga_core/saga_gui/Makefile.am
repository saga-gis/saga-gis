#
# $Id$
#
INCS = -I.. -I.
if DEBUG
DBGFLAGS = -g -DDEBUG
endif
MLIBPATH = "MODULE_LIBRARY_PATH=\"$(libdir)/saga\""
MSHAREPATH = "SHARE_PATH=\"$(prefix)/share/saga\""

BASEFLAGS = -fPIC -D_SAGA_LINUX -D_TYPEDEF_BYTE -D_TYPEDEF_WORD -D_SAGA_DONOTUSE_HARU -D$(MLIBPATH) -D$(MSHAREPATH) $(DBGFLAGS) $(INCS)
if SAGA_UNICODE
AM_CXXFLAGS = $(BASEFLAGS) `wx-config --unicode=yes --cxxflags` -D_SAGA_UNICODE $(GOMPFLAGS)
AM_LDFLAGS = -fPIC `wx-config --unicode=yes --libs adv,aui,base,core,html,net,propgrid,xml`
else
AM_CXXFLAGS = $(BASEFLAGS) `wx-config --unicode=no --cxxflags` $(GOMPFLAGS)
AM_LDFLAGS = -fPIC `wx-config --unicode=no --libs adv,aui,base,core,html,net,propgrid,xml`
endif

bin_PROGRAMS = saga_gui

saga_gui_SOURCES =\
active.cpp\
active_attributes.cpp\
active_description.cpp\
active_history.cpp\
active_HTMLExtraInfo.cpp\
active_legend.cpp\
active_parameters.cpp\
callback.cpp\
data_source.cpp\
data_source_files.cpp\
data_source_odbc.cpp\
dc_helper.cpp\
dlg_about.cpp\
dlg_about_logo.cpp\
dlg_base.cpp\
dlg_colors.cpp\
dlg_colors_control.cpp\
dlg_list_base.cpp\
dlg_list_grid.cpp\
dlg_list_pointcloud.cpp\
dlg_list_shapes.cpp\
dlg_list_table.cpp\
dlg_list_tin.cpp\
dlg_parameters.cpp\
dlg_table.cpp\
dlg_text.cpp\
helper.cpp\
info.cpp\
info_messages.cpp\
parameters_control.cpp\
parameters_properties.cpp\
project.cpp\
res_commands.cpp\
res_controls.cpp\
res_dialogs.cpp\
res_images.cpp\
saga.cpp\
saga_frame.cpp\
saga_frame_droptarget.cpp\
view_base.cpp\
view_histogram.cpp\
view_layout.cpp\
view_layout_control.cpp\
view_layout_info.cpp\
view_layout_printout.cpp\
view_map.cpp\
view_map_3d.cpp\
view_map_3d_image.cpp\
view_map_control.cpp\
view_ruler.cpp\
view_scatterplot.cpp\
view_table.cpp\
view_table_control.cpp\
view_table_diagram.cpp\
wksp.cpp\
wksp_base_control.cpp\
wksp_base_item.cpp\
wksp_base_manager.cpp\
wksp_data_control.cpp\
wksp_data_layers.cpp\
wksp_data_manager.cpp\
wksp_data_menu_file.cpp\
wksp_data_menu_files.cpp\
wksp_grid.cpp\
wksp_grid_manager.cpp\
wksp_grid_system.cpp\
wksp_layer.cpp\
wksp_layer_classify.cpp\
wksp_layer_legend.cpp\
wksp_map.cpp\
wksp_map_buttons.cpp\
wksp_map_control.cpp\
wksp_map_dc.cpp\
wksp_map_layer.cpp\
wksp_map_manager.cpp\
wksp_module.cpp\
wksp_module_control.cpp\
wksp_module_library.cpp\
wksp_module_manager.cpp\
wksp_module_menu.cpp\
wksp_pointcloud.cpp\
wksp_pointcloud_manager.cpp\
wksp_shapes.cpp\
wksp_shapes_edit.cpp\
wksp_shapes_line.cpp\
wksp_shapes_manager.cpp\
wksp_shapes_point.cpp\
wksp_shapes_points.cpp\
wksp_shapes_polygon.cpp\
wksp_shapes_type.cpp\
wksp_table.cpp\
wksp_table_manager.cpp\
wksp_tin.cpp\
wksp_tin_manager.cpp\
active.h\
active_attributes.h\
active_description.h\
active_history.h\
active_HTMLExtraInfo.h\
active_legend.h\
active_parameters.h\
callback.h\
data_source.h\
data_source_files.h\
data_source_odbc.h\
dc_helper.h\
dlg_about.h\
dlg_about_logo.h\
dlg_base.h\
dlg_colors.h\
dlg_colors_control.h\
dlg_list_base.h\
dlg_list_grid.h\
dlg_list_pointcloud.h\
dlg_list_shapes.h\
dlg_list_table.h\
dlg_list_tin.h\
dlg_parameters.h\
dlg_table.h\
dlg_text.h\
helper.h\
info.h\
info_messages.h\
parameters_control.h\
parameters_properties.h\
project.h\
res_commands.h\
res_controls.h\
res_dialogs.h\
res_images.h\
saga.h\
saga_frame.h\
saga_frame_droptarget.h\
view_base.h\
view_histogram.h\
view_layout.h\
view_layout_control.h\
view_layout_info.h\
view_layout_printout.h\
view_map.h\
view_map_3d.h\
view_map_3d_image.h\
view_map_control.h\
view_ruler.h\
view_scatterplot.h\
view_table.h\
view_table_control.h\
view_table_diagram.h\
wksp.h\
wksp_base_control.h\
wksp_base_item.h\
wksp_base_manager.h\
wksp_data_control.h\
wksp_data_layers.h\
wksp_data_manager.h\
wksp_data_menu_file.h\
wksp_data_menu_files.h\
wksp_grid.h\
wksp_grid_manager.h\
wksp_grid_system.h\
wksp_layer.h\
wksp_layer_classify.h\
wksp_layer_legend.h\
wksp_map.h\
wksp_map_buttons.h\
wksp_map_control.h\
wksp_map_dc.h\
wksp_map_layer.h\
wksp_map_manager.h\
wksp_module.h\
wksp_module_control.h\
wksp_module_library.h\
wksp_module_manager.h\
wksp_module_menu.h\
wksp_pointcloud.h\
wksp_pointcloud_manager.h\
wksp_shapes.h\
wksp_shapes_line.h\
wksp_shapes_manager.h\
wksp_shapes_point.h\
wksp_shapes_points.h\
wksp_shapes_polygon.h\
wksp_shapes_type.h\
wksp_table.h\
wksp_table_manager.h\
wksp_tin.h\
wksp_tin_manager.h

saga_gui_LDADD = ../saga_api/libsaga_api.la ../saga_odbc/libsaga_odbc.la

dist-hook:
	 mkdir $(distdir)/res
	 cp -Rp $(srcdir)/res/* $(distdir)/res/

SUBDIRS=man 
