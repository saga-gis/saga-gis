
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    User Interface                     //
//                                                       //
//                    Program: SAGA                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    RES_Images.cpp                     //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/wx.h>
#include <wx/image.h>

#include "res_images.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "./res/xpm/default.xpm"

#include "./res/xpm/saga_icon_32.xpm"
#include "./res/xpm/saga_icon_128.xpm"
#include "./res/xpm/saga_splash.xpm"

#include "./res/xpm/nb_active_attributes.xpm"
#include "./res/xpm/nb_active_description.xpm"
#include "./res/xpm/nb_active_legend.xpm"
#include "./res/xpm/nb_active_history.xpm"
#include "./res/xpm/nb_active_parameters.xpm"
#include "./res/xpm/nb_active_info.xpm"
#include "./res/xpm/nb_info_error.xpm"
#include "./res/xpm/nb_info_execution.xpm"
#include "./res/xpm/nb_info_messages.xpm"
#include "./res/xpm/nb_wksp_data.xpm"
#include "./res/xpm/nb_wksp_maps.xpm"
#include "./res/xpm/nb_wksp_modules.xpm"
#include "./res/xpm/nb_wksp_thumbnails.xpm"
#include "./res/xpm/nb_wksp_treeview.xpm"

#include "./res/xpm/wnd_diagram.xpm"
#include "./res/xpm/wnd_histogram.xpm"
#include "./res/xpm/wnd_layout.xpm"
#include "./res/xpm/wnd_map.xpm"
#include "./res/xpm/wnd_map3d.xpm"
#include "./res/xpm/wnd_scatterplot.xpm"
#include "./res/xpm/wnd_table.xpm"

#include "./res/xpm/crs_edit_point_add.xpm"
#include "./res/xpm/crs_edit_point_move.xpm"
#include "./res/xpm/crs_hand.xpm"
#include "./res/xpm/crs_hand_grap.xpm"
#include "./res/xpm/crs_info.xpm"
#include "./res/xpm/crs_magnifier.xpm"
#include "./res/xpm/crs_select.xpm"

#include "./res/xpm/tb_active.xpm"
#include "./res/xpm/tb_datasource.xpm"
#include "./res/xpm/tb_delete.xpm"
#include "./res/xpm/tb_diagram_parameters.xpm"
#include "./res/xpm/tb_edit_shape_add.xpm"
#include "./res/xpm/tb_edit_shape_add_part.xpm"
#include "./res/xpm/tb_edit_shape_del.xpm"
#include "./res/xpm/tb_edit_shape_del_part.xpm"
#include "./res/xpm/tb_edit_shape_del_point.xpm"
#include "./res/xpm/tb_edit_shape_select.xpm"
#include "./res/xpm/tb_help.xpm"
#include "./res/xpm/tb_histogram_as_table.xpm"
#include "./res/xpm/tb_histogram_cumulative.xpm"
#include "./res/xpm/tb_histogram_gaussian.xpm"
#include "./res/xpm/tb_info.xpm"
#include "./res/xpm/tb_update.xpm"
#include "./res/xpm/tb_clipboard.xpm"
#include "./res/xpm/tb_layout_page_setup.xpm"
#include "./res/xpm/tb_layout_print.xpm"
#include "./res/xpm/tb_layout_print_preview.xpm"
#include "./res/xpm/tb_layout_print_setup.xpm"
#include "./res/xpm/tb_zoom_in.xpm"
#include "./res/xpm/tb_zoom_out.xpm"
#include "./res/xpm/tb_zoom_full.xpm"
#include "./res/xpm/tb_zoom_original.xpm"
#include "./res/xpm/tb_map3d_central_less.xpm"
#include "./res/xpm/tb_map3d_central_more.xpm"
#include "./res/xpm/tb_map3d_exaggerate_less.xpm"
#include "./res/xpm/tb_map3d_exaggerate_more.xpm"
#include "./res/xpm/tb_map3d_interpolated.xpm"
#include "./res/xpm/tb_map3d_properties.xpm"
#include "./res/xpm/tb_map3d_rotate_x_less.xpm"
#include "./res/xpm/tb_map3d_rotate_x_more.xpm"
#include "./res/xpm/tb_map3d_rotate_z_less.xpm"
#include "./res/xpm/tb_map3d_rotate_z_more.xpm"
#include "./res/xpm/tb_map3d_shift_x_less.xpm"
#include "./res/xpm/tb_map3d_shift_x_more.xpm"
#include "./res/xpm/tb_map3d_shift_y_less.xpm"
#include "./res/xpm/tb_map3d_shift_y_more.xpm"
#include "./res/xpm/tb_map3d_shift_z_less.xpm"
#include "./res/xpm/tb_map3d_shift_z_more.xpm"
#include "./res/xpm/tb_map3d_stereo.xpm"
#include "./res/xpm/tb_map_3d_show.xpm"
#include "./res/xpm/tb_map_layout_show.xpm"
#include "./res/xpm/tb_map_legend_horizontal.xpm"
#include "./res/xpm/tb_map_legend_vertical.xpm"
#include "./res/xpm/tb_map_mode_distance.xpm"
#include "./res/xpm/tb_map_mode_pan.xpm"
#include "./res/xpm/tb_map_mode_select.xpm"
#include "./res/xpm/tb_map_mode_zoom.xpm"
#include "./res/xpm/tb_map_scalebar.xpm"
#include "./res/xpm/tb_map_north_arrow.xpm"
#include "./res/xpm/tb_map_synchronize.xpm"
#include "./res/xpm/tb_map_zoom_active.xpm"
#include "./res/xpm/tb_map_pan_active.xpm"
#include "./res/xpm/tb_map_zoom_full.xpm"
#include "./res/xpm/tb_map_zoom_last.xpm"
#include "./res/xpm/tb_map_zoom_next.xpm"
#include "./res/xpm/tb_map_zoom_selection.xpm"
#include "./res/xpm/tb_map_crosshair.xpm"
#include "./res/xpm/tb_new_project.xpm"
#include "./res/xpm/tb_open.xpm"
#include "./res/xpm/tb_open_grid.xpm"
#include "./res/xpm/tb_open_module.xpm"
#include "./res/xpm/tb_open_shapes.xpm"
#include "./res/xpm/tb_open_table.xpm"
#include "./res/xpm/tb_open_tin.xpm"
#include "./res/xpm/tb_save.xpm"
#include "./res/xpm/tb_scatterplot_parameters.xpm"
#include "./res/xpm/tb_show.xpm"
#include "./res/xpm/tb_table_col_add.xpm"
#include "./res/xpm/tb_table_col_del.xpm"
#include "./res/xpm/tb_table_row_add.xpm"
#include "./res/xpm/tb_table_row_del.xpm"
#include "./res/xpm/tb_table_row_del_all.xpm"
#include "./res/xpm/tb_table_row_ins.xpm"
#include "./res/xpm/tb_table_selection_only.xpm"
#include "./res/xpm/tb_wksp.xpm"

#include "./res/xpm/wksp_data_manager.xpm"
#include "./res/xpm/wksp_grid.xpm"
#include "./res/xpm/wksp_grid_manager.xpm"
#include "./res/xpm/wksp_grid_system.xpm"
#include "./res/xpm/wksp_map.xpm"
#include "./res/xpm/wksp_map_manager.xpm"
#include "./res/xpm/wksp_map_graticule.xpm"
#include "./res/xpm/wksp_module.xpm"
#include "./res/xpm/wksp_module_library.xpm"
#include "./res/xpm/wksp_module_manager.xpm"
#include "./res/xpm/wksp_noitems.xpm"
#include "./res/xpm/wksp_odbc_sources.xpm"
#include "./res/xpm/wksp_odbc_source_on.xpm"
#include "./res/xpm/wksp_odbc_source_off.xpm"
#include "./res/xpm/wksp_shapes_line.xpm"
#include "./res/xpm/wksp_shapes_manager.xpm"
#include "./res/xpm/wksp_shapes_point.xpm"
#include "./res/xpm/wksp_shapes_points.xpm"
#include "./res/xpm/wksp_shapes_polygon.xpm"
#include "./res/xpm/wksp_table.xpm"
#include "./res/xpm/wksp_table_manager.xpm"
#include "./res/xpm/wksp_tin.xpm"
#include "./res/xpm/wksp_tin_manager.xpm"

//---------------------------------------------------------
const char ** _Get_XPM(int ID_IMG)
{
	switch( ID_IMG )
	{
	default                              : return( default_xpm );

	case ID_IMG_SAGA_ICON_32             : return( saga_icon_32_xpm );
	case ID_IMG_SAGA_ICON                : return( saga_icon_128_xpm );
	case ID_IMG_SAGA_SPLASH              : return( saga_splash_xpm );

	case ID_IMG_NB_ACTIVE_PARAMETERS     : return( nb_active_parameters_xpm );
	case ID_IMG_NB_ACTIVE_DESCRIPTION    : return( nb_active_description_xpm );
	case ID_IMG_NB_ACTIVE_HISTORY        : return( nb_active_history_xpm );
	case ID_IMG_NB_ACTIVE_LEGEND         : return( nb_active_legend_xpm );
	case ID_IMG_NB_ACTIVE_ATTRIBUTES     : return( nb_active_attributes_xpm );
	case ID_IMG_NB_ACTIVE_INFO           : return( nb_active_info_xpm );

	case ID_IMG_NB_DATA_SOURCE_FILES     : return( tb_wksp_xpm );
	case ID_IMG_NB_DATA_SOURCE_DATABASE  : return( wksp_odbc_sources_xpm );
	case ID_IMG_NB_DATA_SOURCE_WEBSERVICE: return( tb_wksp_xpm );

	case ID_IMG_NB_INFO_ERROR            : return( nb_info_error_xpm );
	case ID_IMG_NB_INFO_EXECUTION        : return( nb_info_execution_xpm );
	case ID_IMG_NB_INFO_MESSAGES         : return( nb_info_messages_xpm );

	case ID_IMG_NB_WKSP_DATA             : return( nb_wksp_data_xpm );
	case ID_IMG_NB_WKSP_MAPS             : return( nb_wksp_maps_xpm );
	case ID_IMG_NB_WKSP_TOOLS            : return( nb_wksp_modules_xpm );
	case ID_IMG_NB_WKSP_THUMBNAILS       : return( nb_wksp_thumbnails_xpm );
	case ID_IMG_NB_WKSP_TREEVIEW         : return( nb_wksp_treeview_xpm );

	case ID_IMG_WND_DIAGRAM              : return( wnd_diagram_xpm );
	case ID_IMG_WND_HISTOGRAM            : return( wnd_histogram_xpm );
	case ID_IMG_WND_LAYOUT               : return( wnd_layout_xpm );
	case ID_IMG_WND_MAP                  : return( wnd_map_xpm );
	case ID_IMG_WND_MAP3D                : return( wnd_map3d_xpm );
	case ID_IMG_WND_SCATTERPLOT          : return( wnd_scatterplot_xpm );
	case ID_IMG_WND_TABLE                : return( wnd_table_xpm );

	case ID_IMG_CRS_EDIT_POINT_ADD       : return( crs_edit_point_add_xpm );
	case ID_IMG_CRS_EDIT_POINT_MOVE      : return( crs_edit_point_move_xpm );
	case ID_IMG_CRS_HAND                 : return( crs_hand_xpm );
	case ID_IMG_CRS_HAND_GRAP            : return( crs_hand_grap_xpm );
	case ID_IMG_CRS_INFO                 : return( crs_info_xpm );
	case ID_IMG_CRS_MAGNIFIER            : return( crs_magnifier_xpm );
	case ID_IMG_CRS_SELECT               : return( crs_select_xpm );

	case ID_IMG_TB_ACTIVE                : return( tb_active_xpm );
	case ID_IMG_TB_DATASOURCE            : return( tb_datasource_xpm );
	case ID_IMG_TB_DELETE                : return( tb_delete_xpm );
	case ID_IMG_TB_DIAGRAM_PARAMETERS    : return( tb_diagram_parameters_xpm );
	case ID_IMG_TB_EDIT_SHAPE_ADD        : return( tb_edit_shape_add_xpm );
	case ID_IMG_TB_EDIT_SHAPE_ADD_PART   : return( tb_edit_shape_add_part_xpm );
	case ID_IMG_TB_EDIT_SHAPE_DEL        : return( tb_edit_shape_del_xpm );
	case ID_IMG_TB_EDIT_SHAPE_DEL_PART   : return( tb_edit_shape_del_part_xpm );
	case ID_IMG_TB_EDIT_SHAPE_DEL_POINT  : return( tb_edit_shape_del_point_xpm );
	case ID_IMG_TB_EDIT_SHAPE_SELECT     : return( tb_edit_shape_select_xpm );
	case ID_IMG_TB_HELP                  : return( tb_help_xpm );
	case ID_IMG_TB_HISTOGRAM_AS_TABLE    : return( tb_histogram_as_table_xpm );
	case ID_IMG_TB_HISTOGRAM_CUMULATIVE  : return( tb_histogram_cumulative_xpm );
	case ID_IMG_TB_HISTOGRAM_GAUSSIAN    : return( tb_histogram_gaussian_xpm );
	case ID_IMG_TB_INFO                  : return( tb_info_xpm );
	case ID_IMG_TB_UPDATE                : return( tb_update_xpm );
	case ID_IMG_TB_CLIPBOARD             : return( tb_clipboard_xpm );
	case ID_IMG_TB_LAYOUT_PAGE_SETUP     : return( tb_layout_page_setup_xpm );
	case ID_IMG_TB_LAYOUT_PRINT          : return( tb_layout_print_xpm );
	case ID_IMG_TB_LAYOUT_PRINT_PREVIEW  : return( tb_layout_print_preview_xpm );
	case ID_IMG_TB_LAYOUT_PRINT_SETUP    : return( tb_layout_print_setup_xpm );
	case ID_IMG_TB_ZOOM_IN               : return( tb_zoom_in_xpm );
	case ID_IMG_TB_ZOOM_OUT              : return( tb_zoom_out_xpm );
	case ID_IMG_TB_ZOOM_FULL             : return( tb_zoom_full_xpm );
	case ID_IMG_TB_ZOOM_ORIGINAL         : return( tb_zoom_original_xpm );
	case ID_IMG_TB_MAP3D_CENTRAL_LESS    : return( tb_map3d_central_less_xpm );
	case ID_IMG_TB_MAP3D_CENTRAL_MORE    : return( tb_map3d_central_more_xpm );
	case ID_IMG_TB_MAP3D_EXAGGERATE_LESS : return( tb_map3d_exaggerate_less_xpm );
	case ID_IMG_TB_MAP3D_EXAGGERATE_MORE : return( tb_map3d_exaggerate_more_xpm );
	case ID_IMG_TB_MAP3D_INTERPOLATED    : return( tb_map3d_interpolated_xpm );
	case ID_IMG_TB_MAP3D_PROPERTIES      : return( tb_map3d_properties_xpm );
	case ID_IMG_TB_MAP3D_ROTATE_X_LESS   : return( tb_map3d_rotate_x_less_xpm );
	case ID_IMG_TB_MAP3D_ROTATE_X_MORE   : return( tb_map3d_rotate_x_more_xpm );
	case ID_IMG_TB_MAP3D_ROTATE_Z_LESS   : return( tb_map3d_rotate_z_less_xpm );
	case ID_IMG_TB_MAP3D_ROTATE_Z_MORE   : return( tb_map3d_rotate_z_more_xpm );
	case ID_IMG_TB_MAP3D_SHIFT_X_LESS    : return( tb_map3d_shift_x_less_xpm );
	case ID_IMG_TB_MAP3D_SHIFT_X_MORE    : return( tb_map3d_shift_x_more_xpm );
	case ID_IMG_TB_MAP3D_SHIFT_Y_LESS    : return( tb_map3d_shift_y_less_xpm );
	case ID_IMG_TB_MAP3D_SHIFT_Y_MORE    : return( tb_map3d_shift_y_more_xpm );
	case ID_IMG_TB_MAP3D_SHIFT_Z_LESS    : return( tb_map3d_shift_z_less_xpm );
	case ID_IMG_TB_MAP3D_SHIFT_Z_MORE    : return( tb_map3d_shift_z_more_xpm );
	case ID_IMG_TB_MAP3D_STEREO          : return( tb_map3d_stereo_xpm );
	case ID_IMG_TB_MAP_3D_SHOW           : return( tb_map_3d_show_xpm );
	case ID_IMG_TB_MAP_LAYOUT_SHOW       : return( tb_map_layout_show_xpm );
	case ID_IMG_TB_MAP_LEGEND_HORIZONTAL : return( tb_map_legend_horizontal_xpm );
	case ID_IMG_TB_MAP_LEGEND_VERTICAL   : return( tb_map_legend_vertical_xpm );
	case ID_IMG_TB_MAP_MODE_DISTANCE     : return( tb_map_mode_distance_xpm );
	case ID_IMG_TB_MAP_MODE_PAN          : return( tb_map_mode_pan_xpm );
	case ID_IMG_TB_MAP_MODE_SELECT       : return( tb_map_mode_select_xpm );
	case ID_IMG_TB_MAP_MODE_ZOOM         : return( tb_map_mode_zoom_xpm );
    case ID_IMG_TB_MAP_SCALEBAR          : return( tb_map_scalebar_xpm );
	case ID_IMG_TB_MAP_NORTH_ARROW       : return( tb_map_north_arrow_xpm );
	case ID_IMG_TB_MAP_SYNCHRONIZE       : return( tb_map_synchronize_xpm );
	case ID_IMG_TB_MAP_ZOOM_ACTIVE       : return( tb_map_zoom_active_xpm );
	case ID_IMG_TB_MAP_PAN_ACTIVE        : return( tb_map_pan_active_xpm );
	case ID_IMG_TB_MAP_ZOOM_FULL         : return( tb_map_zoom_full_xpm );
	case ID_IMG_TB_MAP_ZOOM_LAST         : return( tb_map_zoom_last_xpm );
	case ID_IMG_TB_MAP_ZOOM_NEXT         : return( tb_map_zoom_next_xpm );
	case ID_IMG_TB_MAP_ZOOM_SELECTION    : return( tb_map_zoom_selection_xpm );
	case ID_IMG_TB_MAP_CROSSHAIR         : return( tb_map_crosshair_xpm );
	case ID_IMG_TB_NEW_PROJECT           : return( tb_new_project_xpm );
	case ID_IMG_TB_OPEN                  : return( tb_open_xpm );
	case ID_IMG_TB_OPEN_GRID             : return( tb_open_grid_xpm );
	case ID_IMG_TB_OPEN_TOOL             : return( tb_open_module_xpm );
	case ID_IMG_TB_OPEN_SHAPES           : return( tb_open_shapes_xpm );
	case ID_IMG_TB_OPEN_TABLE            : return( tb_open_table_xpm );
	case ID_IMG_TB_OPEN_TIN              : return( tb_open_tin_xpm );
	case ID_IMG_TB_SAVE                  : return( tb_save_xpm );
	case ID_IMG_TB_SCATTERPLOT_PARAMETERS: return( tb_scatterplot_parameters_xpm );
	case ID_IMG_TB_SHOW                  : return( tb_show_xpm );
	case ID_IMG_TB_TABLE_COL_ADD         : return( tb_table_col_add_xpm );
	case ID_IMG_TB_TABLE_COL_DEL         : return( tb_table_col_del_xpm );
	case ID_IMG_TB_TABLE_ROW_ADD         : return( tb_table_row_add_xpm );
	case ID_IMG_TB_TABLE_ROW_DEL         : return( tb_table_row_del_xpm );
	case ID_IMG_TB_TABLE_ROW_DEL_ALL     : return( tb_table_row_del_all_xpm );
	case ID_IMG_TB_TABLE_ROW_INS         : return( tb_table_row_ins_xpm );
	case ID_IMG_TB_TABLE_SELECTION_ONLY  : return( tb_table_selection_only_xpm );
	case ID_IMG_TB_WKSP                  : return( tb_wksp_xpm );

	case ID_IMG_WKSP_DATA_MANAGER        : return( wksp_data_manager_xpm );
	case ID_IMG_WKSP_GRID_MANAGER        : return( wksp_grid_manager_xpm );
	case ID_IMG_WKSP_GRID_SYSTEM         : return( wksp_grid_system_xpm );
	case ID_IMG_WKSP_GRID                : return( wksp_grid_xpm );
	case ID_IMG_WKSP_GRIDS               : return( wksp_grid_manager_xpm );
	case ID_IMG_WKSP_MAP                 : return( wksp_map_xpm );
	case ID_IMG_WKSP_MAP_GRATICULE       : return( wksp_map_graticule_xpm );
	case ID_IMG_WKSP_MAP_BASEMAP         : return( wnd_map_xpm );
	case ID_IMG_WKSP_MAP_MANAGER         : return( wksp_map_manager_xpm );
	case ID_IMG_WKSP_TOOL                : return( wksp_module_xpm );
	case ID_IMG_WKSP_TOOL_CHAIN          : return( wksp_module_xpm );
	case ID_IMG_WKSP_TOOL_LIBRARY        : return( wksp_module_library_xpm );
	case ID_IMG_WKSP_TOOL_GROUP          : return( wksp_module_library_xpm );
	case ID_IMG_WKSP_TOOL_MANAGER        : return( wksp_module_manager_xpm );
	case ID_IMG_WKSP_NOITEMS             : return( wksp_noitems_xpm );
	case ID_IMG_WKSP_DB_SOURCES          : return( wksp_odbc_sources_xpm );
	case ID_IMG_WKSP_DB_SOURCE_ON        : return( wksp_odbc_source_on_xpm );
	case ID_IMG_WKSP_DB_SOURCE_OFF       : return( wksp_odbc_source_off_xpm );
	case ID_IMG_WKSP_DB_TABLE            : return( wksp_table_xpm );
	case ID_IMG_WKSP_SHAPES_LINE         : return( wksp_shapes_line_xpm );
	case ID_IMG_WKSP_SHAPES_MANAGER      : return( wksp_shapes_manager_xpm );
	case ID_IMG_WKSP_SHAPES_POINT        : return( wksp_shapes_point_xpm );
	case ID_IMG_WKSP_SHAPES_POINTS       : return( wksp_shapes_points_xpm );
	case ID_IMG_WKSP_SHAPES_POLYGON      : return( wksp_shapes_polygon_xpm );
	case ID_IMG_WKSP_TABLE               : return( wksp_table_xpm );
	case ID_IMG_WKSP_TABLE_MANAGER       : return( wksp_table_manager_xpm );
	case ID_IMG_WKSP_TIN                 : return( wksp_tin_xpm );
	case ID_IMG_WKSP_TIN_MANAGER         : return( wksp_tin_manager_xpm );
	case ID_IMG_WKSP_POINTCLOUD          : return( wksp_shapes_points_xpm );
	case ID_IMG_WKSP_POINTCLOUD_MANAGER  : return( wksp_shapes_points_xpm );
	}
}

//---------------------------------------------------------
wxImage		IMG_Get_Image(int ID_IMG)
{
	return( wxImage(_Get_XPM(ID_IMG)) );
}

wxImage		IMG_Get_Image(int ID_IMG, const wxSize &Size)
{
	return( IMG_Get_Image(ID_IMG).Rescale(Size.GetWidth(), Size.GetHeight()) );
}

//---------------------------------------------------------
wxIcon		IMG_Get_Icon(int ID_IMG)
{
	return( wxIcon(_Get_XPM(ID_IMG)) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef WITH_WXBMPBUNDLE

//---------------------------------------------------------
wxBitmap	IMG_Get_Bitmap(int ID_IMG)
{
	return( wxBitmap(_Get_XPM(ID_IMG)) );
}

wxBitmap	IMG_Get_Bitmap(int ID_IMG, const wxSize &Size)
{
	return( wxBitmap(IMG_Get_Image(ID_IMG, Size)) );
}

//---------------------------------------------------------
wxCursor	IMG_Get_Cursor(int ID_IMG)
{
	wxImage	Image(_Get_XPM(ID_IMG));

	Image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 15);
	Image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 15);

	return( wxCursor(Image) );
}

//---------------------------------------------------------
#endif // #ifndef WITH_WXBMPBUNDLE


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifdef WITH_WXBMPBUNDLE

//---------------------------------------------------------
#include "./res/svg/default.svg.h"

#include "./res/svg/saga_icon.svg.h"
//#include "./res/svg/saga_splash.svg.h"

#include "./res/svg/nb_active_attributes.svg.h"
#include "./res/svg/nb_active_description.svg.h"
#include "./res/svg/nb_active_legend.svg.h"
#include "./res/svg/nb_active_history.svg.h"
#include "./res/svg/nb_active_parameters.svg.h"
//#include "./res/svg/nb_active_info.svg.h"
#include "./res/svg/nb_info_error.svg.h"
#include "./res/svg/nb_info_execution.svg.h"
#include "./res/svg/nb_info_messages.svg.h"
#include "./res/svg/nb_wksp_data.svg.h"
#include "./res/svg/nb_wksp_maps.svg.h"
#include "./res/svg/nb_wksp_modules.svg.h"
#include "./res/svg/nb_wksp_thumbnails.svg.h"
//#include "./res/svg/nb_wksp_treeview.svg.h"

#include "./res/svg/wnd_diagram.svg.h"
#include "./res/svg/wnd_histogram.svg.h"
//#include "./res/svg/wnd_layout.svg.h"
//#include "./res/svg/wnd_map.svg.h"
#include "./res/svg/wnd_map3d.svg.h"
//#include "./res/svg/wnd_scatterplot.svg.h"
//#include "./res/svg/wnd_table.svg.h"

//#include "./res/svg/crs_edit_point_add.svg.h"
//#include "./res/svg/crs_edit_point_move.svg.h"
//#include "./res/svg/crs_hand.svg.h"
//#include "./res/svg/crs_hand_grap.svg.h"
//#include "./res/svg/crs_info.svg.h"
#include "./res/svg/crs_magnifier.svg.h"
//#include "./res/svg/crs_select.svg.h"

#include "./res/svg/tb_active.svg.h"
#include "./res/svg/tb_datasource.svg.h"
//#include "./res/svg/tb_delete.svg.h"
#include "./res/svg/tb_diagram_parameters.svg.h"
//#include "./res/svg/tb_edit_shape_add.svg.h"
//#include "./res/svg/tb_edit_shape_add_part.svg.h"
//#include "./res/svg/tb_edit_shape_del.svg.h"
//#include "./res/svg/tb_edit_shape_del_part.svg.h"
//#include "./res/svg/tb_edit_shape_del_point.svg.h"
//#include "./res/svg/tb_edit_shape_select.svg.h"
#include "./res/svg/tb_help.svg.h"
//#include "./res/svg/tb_histogram_as_table.svg.h"
//#include "./res/svg/tb_histogram_cumulative.svg.h"
//#include "./res/svg/tb_histogram_gaussian.svg.h"
#include "./res/svg/tb_info.svg.h"
#include "./res/svg/tb_update.svg.h"
#include "./res/svg/tb_clipboard.svg.h"
//#include "./res/svg/tb_layout_page_setup.svg.h"
#include "./res/svg/tb_layout_print.svg.h"
//#include "./res/svg/tb_layout_print_preview.svg.h"
//#include "./res/svg/tb_layout_print_setup.svg.h"
#include "./res/svg/tb_zoom_in.svg.h"
#include "./res/svg/tb_zoom_out.svg.h"
#include "./res/svg/tb_zoom_full.svg.h"
#include "./res/svg/tb_zoom_original.svg.h"
//#include "./res/svg/tb_map3d_central_less.svg.h"
#include "./res/svg/tb_map3d_central_more.svg.h"
#include "./res/svg/tb_map3d_exaggerate_less.svg.h"
#include "./res/svg/tb_map3d_exaggerate_more.svg.h"
//#include "./res/svg/tb_map3d_interpolated.svg.h"
#include "./res/svg/tb_map3d_properties.svg.h"
//#include "./res/svg/tb_map3d_rotate_x_less.svg.h"
//#include "./res/svg/tb_map3d_rotate_x_more.svg.h"
//#include "./res/svg/tb_map3d_rotate_z_less.svg.h"
//#include "./res/svg/tb_map3d_rotate_z_more.svg.h"
#include "./res/svg/tb_map3d_shift_x_less.svg.h"
#include "./res/svg/tb_map3d_shift_x_more.svg.h"
#include "./res/svg/tb_map3d_shift_y_less.svg.h"
#include "./res/svg/tb_map3d_shift_y_more.svg.h"
//#include "./res/svg/tb_map3d_shift_z_less.svg.h"
//#include "./res/svg/tb_map3d_shift_z_more.svg.h"
//#include "./res/svg/tb_map3d_stereo.svg.h"
#include "./res/svg/tb_map_3d_show.svg.h"
#include "./res/svg/tb_map_layout_show.svg.h"
//#include "./res/svg/tb_map_legend_horizontal.svg.h"
//#include "./res/svg/tb_map_legend_vertical.svg.h"
#include "./res/svg/tb_map_mode_distance.svg.h"
#include "./res/svg/tb_map_mode_pan.svg.h"
#include "./res/svg/tb_map_mode_select.svg.h"
#include "./res/svg/tb_map_mode_zoom.svg.h"
#include "./res/svg/tb_map_scalebar.svg.h"
#include "./res/svg/tb_map_north_arrow.svg.h"
#include "./res/svg/tb_map_synchronize.svg.h"
#include "./res/svg/tb_map_zoom_active.svg.h"
#include "./res/svg/tb_map_pan_active.svg.h"
#include "./res/svg/tb_map_zoom_full.svg.h"
#include "./res/svg/tb_map_zoom_last.svg.h"
#include "./res/svg/tb_map_zoom_next.svg.h"
#include "./res/svg/tb_map_zoom_selection.svg.h"
#include "./res/svg/tb_map_crosshair.svg.h"
#include "./res/svg/tb_new_project.svg.h"
#include "./res/svg/tb_open.svg.h"
//#include "./res/svg/tb_open_grid.svg.h"
#include "./res/svg/tb_open_module.svg.h"
//#include "./res/svg/tb_open_shapes.svg.h"
//#include "./res/svg/tb_open_table.svg.h"
//#include "./res/svg/tb_open_tin.svg.h"
#include "./res/svg/tb_save.svg.h"
//#include "./res/svg/tb_scatterplot_parameters.svg.h"
#include "./res/svg/tb_show.svg.h"
//#include "./res/svg/tb_table_col_add.svg.h"
//#include "./res/svg/tb_table_col_del.svg.h"
//#include "./res/svg/tb_table_row_add.svg.h"
//#include "./res/svg/tb_table_row_del.svg.h"
//#include "./res/svg/tb_table_row_del_all.svg.h"
//#include "./res/svg/tb_table_row_ins.svg.h"
//#include "./res/svg/tb_table_selection_only.svg.h"
#include "./res/svg/tb_wksp.svg.h"

#include "./res/svg/wksp_data_manager.svg.h"
//#include "./res/svg/wksp_grid.svg.h"
//#include "./res/svg/wksp_grid_manager.svg.h"
//#include "./res/svg/wksp_grid_system.svg.h"
//#include "./res/svg/wksp_map.svg.h"
//#include "./res/svg/wksp_map_manager.svg.h"
#include "./res/svg/wksp_map_graticule.svg.h"
#include "./res/svg/wksp_module.svg.h"
#include "./res/svg/wksp_module_library.svg.h"
#include "./res/svg/wksp_module_manager.svg.h"
//#include "./res/svg/wksp_noitems.svg.h"
#include "./res/svg/wksp_odbc_sources.svg.h"
//#include "./res/svg/wksp_odbc_source_on.svg.h"
//#include "./res/svg/wksp_odbc_source_off.svg.h"
//#include "./res/svg/wksp_shapes_line.svg.h"
#include "./res/svg/wksp_shapes_manager.svg.h"
//#include "./res/svg/wksp_shapes_point.svg.h"
//#include "./res/svg/wksp_shapes_points.svg.h"
//#include "./res/svg/wksp_shapes_polygon.svg.h"
//#include "./res/svg/wksp_table.svg.h"
//#include "./res/svg/wksp_table_manager.svg.h"
//#include "./res/svg/wksp_tin.svg.h"
//#include "./res/svg/wksp_tin_manager.svg.h"

//---------------------------------------------------------
const char * _Get_SVG(int ID_IMG)
{
	switch( ID_IMG )
	{
//	default                              : return( svg_default );

	case ID_IMG_SAGA_ICON                : return( svg_saga_icon );
//	case ID_IMG_SAGA_SPLASH              : return( svg_saga_splash );

	case ID_IMG_NB_ACTIVE_PARAMETERS     : return( svg_nb_active_parameters );
	case ID_IMG_NB_ACTIVE_DESCRIPTION    : return( svg_nb_active_description );
	case ID_IMG_NB_ACTIVE_HISTORY        : return( svg_nb_active_history );
	case ID_IMG_NB_ACTIVE_LEGEND         : return( svg_nb_active_legend );
	case ID_IMG_NB_ACTIVE_ATTRIBUTES     : return( svg_nb_active_attributes );
//	case ID_IMG_NB_ACTIVE_INFO           : return( svg_nb_active_info );

	case ID_IMG_NB_DATA_SOURCE_FILES     : return( svg_tb_wksp );
	case ID_IMG_NB_DATA_SOURCE_DATABASE  : return( svg_wksp_odbc_sources );
	case ID_IMG_NB_DATA_SOURCE_WEBSERVICE: return( svg_tb_wksp );

	case ID_IMG_NB_INFO_ERROR            : return( svg_nb_info_error );
	case ID_IMG_NB_INFO_EXECUTION        : return( svg_nb_info_execution );
	case ID_IMG_NB_INFO_MESSAGES         : return( svg_nb_info_messages );

	case ID_IMG_NB_WKSP_DATA             : return( svg_nb_wksp_data );
	case ID_IMG_NB_WKSP_MAPS             : return( svg_nb_wksp_maps );
	case ID_IMG_NB_WKSP_TOOLS            : return( svg_nb_wksp_modules );
	case ID_IMG_NB_WKSP_THUMBNAILS       : return( svg_nb_wksp_thumbnails );
//	case ID_IMG_NB_WKSP_TREEVIEW         : return( svg_nb_wksp_treeview );

	case ID_IMG_WND_DIAGRAM              : return( svg_wnd_diagram );
	case ID_IMG_WND_HISTOGRAM            : return( svg_wnd_histogram );
//	case ID_IMG_WND_LAYOUT               : return( svg_wnd_layout );
//	case ID_IMG_WND_MAP                  : return( svg_wnd_map );
	case ID_IMG_WND_MAP3D                : return( svg_wnd_map3d );
//	case ID_IMG_WND_SCATTERPLOT          : return( svg_wnd_scatterplot );
//	case ID_IMG_WND_TABLE                : return( svg_wnd_table );

//	case ID_IMG_CRS_EDIT_POINT_ADD       : return( svg_crs_edit_point_add );
//	case ID_IMG_CRS_EDIT_POINT_MOVE      : return( svg_crs_edit_point_move );
//	case ID_IMG_CRS_HAND                 : return( svg_crs_hand );
//	case ID_IMG_CRS_HAND_GRAP            : return( svg_crs_hand_grap );
//	case ID_IMG_CRS_INFO                 : return( svg_crs_info );
	case ID_IMG_CRS_MAGNIFIER            : return( svg_crs_magnifier );
//	case ID_IMG_CRS_SELECT               : return( svg_crs_select );

	case ID_IMG_TB_ACTIVE                : return( svg_tb_active );
	case ID_IMG_TB_DATASOURCE            : return( svg_tb_datasource );
//	case ID_IMG_TB_DELETE                : return( svg_tb_delete );
	case ID_IMG_TB_DIAGRAM_PARAMETERS    : return( svg_tb_diagram_parameters );
//	case ID_IMG_TB_EDIT_SHAPE_ADD        : return( svg_tb_edit_shape_add );
//	case ID_IMG_TB_EDIT_SHAPE_ADD_PART   : return( svg_tb_edit_shape_add_part );
//	case ID_IMG_TB_EDIT_SHAPE_DEL        : return( svg_tb_edit_shape_del );
//	case ID_IMG_TB_EDIT_SHAPE_DEL_PART   : return( svg_tb_edit_shape_del_part );
//	case ID_IMG_TB_EDIT_SHAPE_DEL_POINT  : return( svg_tb_edit_shape_del_point );
//	case ID_IMG_TB_EDIT_SHAPE_SELECT     : return( svg_tb_edit_shape_select );
	case ID_IMG_TB_HELP                  : return( svg_tb_help );
//	case ID_IMG_TB_HISTOGRAM_AS_TABLE    : return( svg_tb_histogram_as_table );
//	case ID_IMG_TB_HISTOGRAM_CUMULATIVE  : return( svg_tb_histogram_cumulative );
//	case ID_IMG_TB_HISTOGRAM_GAUSSIAN    : return( svg_tb_histogram_gaussian );
	case ID_IMG_TB_INFO                  : return( svg_tb_info );
	case ID_IMG_TB_UPDATE                : return( svg_tb_update );
	case ID_IMG_TB_CLIPBOARD             : return( svg_tb_clipboard );
//	case ID_IMG_TB_LAYOUT_PAGE_SETUP     : return( svg_tb_layout_page_setup );
	case ID_IMG_TB_LAYOUT_PRINT          : return( svg_tb_layout_print );
//	case ID_IMG_TB_LAYOUT_PRINT_PREVIEW  : return( svg_tb_layout_print_preview );
//	case ID_IMG_TB_LAYOUT_PRINT_SETUP    : return( svg_tb_layout_print_setup );
	case ID_IMG_TB_ZOOM_IN               : return( svg_tb_zoom_in );
	case ID_IMG_TB_ZOOM_OUT              : return( svg_tb_zoom_out );
	case ID_IMG_TB_ZOOM_FULL             : return( svg_tb_zoom_full );
	case ID_IMG_TB_ZOOM_ORIGINAL         : return( svg_tb_zoom_original );
//	case ID_IMG_TB_MAP3D_CENTRAL_LESS    : return( svg_tb_map3d_central_less );
	case ID_IMG_TB_MAP3D_CENTRAL_MORE    : return( svg_tb_map3d_central_more );
	case ID_IMG_TB_MAP3D_EXAGGERATE_LESS : return( svg_tb_map3d_exaggerate_less );
	case ID_IMG_TB_MAP3D_EXAGGERATE_MORE : return( svg_tb_map3d_exaggerate_more );
//	case ID_IMG_TB_MAP3D_INTERPOLATED    : return( svg_tb_map3d_interpolated );
	case ID_IMG_TB_MAP3D_PROPERTIES      : return( svg_tb_map3d_properties );
//	case ID_IMG_TB_MAP3D_ROTATE_X_LESS   : return( svg_tb_map3d_rotate_x_less );
//	case ID_IMG_TB_MAP3D_ROTATE_X_MORE   : return( svg_tb_map3d_rotate_x_more );
//	case ID_IMG_TB_MAP3D_ROTATE_Z_LESS   : return( svg_tb_map3d_rotate_z_less );
//	case ID_IMG_TB_MAP3D_ROTATE_Z_MORE   : return( svg_tb_map3d_rotate_z_more );
	case ID_IMG_TB_MAP3D_SHIFT_X_LESS    : return( svg_tb_map3d_shift_x_less );
	case ID_IMG_TB_MAP3D_SHIFT_X_MORE    : return( svg_tb_map3d_shift_x_more );
	case ID_IMG_TB_MAP3D_SHIFT_Y_LESS    : return( svg_tb_map3d_shift_y_less );
	case ID_IMG_TB_MAP3D_SHIFT_Y_MORE    : return( svg_tb_map3d_shift_y_more );
//	case ID_IMG_TB_MAP3D_SHIFT_Z_LESS    : return( svg_tb_map3d_shift_z_less );
//	case ID_IMG_TB_MAP3D_SHIFT_Z_MORE    : return( svg_tb_map3d_shift_z_more );
//	case ID_IMG_TB_MAP3D_STEREO          : return( svg_tb_map3d_stereo );
	case ID_IMG_TB_MAP_3D_SHOW           : return( svg_tb_map_3d_show );
	case ID_IMG_TB_MAP_LAYOUT_SHOW       : return( svg_tb_map_layout_show );
//	case ID_IMG_TB_MAP_LEGEND_HORIZONTAL : return( svg_tb_map_legend_horizontal );
//	case ID_IMG_TB_MAP_LEGEND_VERTICAL   : return( svg_tb_map_legend_vertical );
	case ID_IMG_TB_MAP_MODE_DISTANCE     : return( svg_tb_map_mode_distance );
	case ID_IMG_TB_MAP_MODE_PAN          : return( svg_tb_map_mode_pan );
	case ID_IMG_TB_MAP_MODE_SELECT       : return( svg_tb_map_mode_select );
	case ID_IMG_TB_MAP_MODE_ZOOM         : return( svg_tb_map_mode_zoom );
	case ID_IMG_TB_MAP_SCALEBAR          : return( svg_tb_map_scalebar );
	case ID_IMG_TB_MAP_NORTH_ARROW       : return( svg_tb_map_north_arrow );
	case ID_IMG_TB_MAP_SYNCHRONIZE       : return( svg_tb_map_synchronize );
	case ID_IMG_TB_MAP_ZOOM_ACTIVE       : return( svg_tb_map_zoom_active );
	case ID_IMG_TB_MAP_PAN_ACTIVE        : return( svg_tb_map_pan_active );
	case ID_IMG_TB_MAP_ZOOM_FULL         : return( svg_tb_map_zoom_full );
	case ID_IMG_TB_MAP_ZOOM_LAST         : return( svg_tb_map_zoom_last );
	case ID_IMG_TB_MAP_ZOOM_NEXT         : return( svg_tb_map_zoom_next );
	case ID_IMG_TB_MAP_ZOOM_SELECTION    : return( svg_tb_map_zoom_selection );
	case ID_IMG_TB_MAP_CROSSHAIR         : return( svg_tb_map_crosshair );
	case ID_IMG_TB_NEW_PROJECT           : return( svg_tb_new_project );
	case ID_IMG_TB_OPEN                  : return( svg_tb_open );
//	case ID_IMG_TB_OPEN_GRID             : return( svg_tb_open_grid );
	case ID_IMG_TB_OPEN_TOOL             : return( svg_tb_open_module );
//	case ID_IMG_TB_OPEN_SHAPES           : return( svg_tb_open_shapes );
//	case ID_IMG_TB_OPEN_TABLE            : return( svg_tb_open_table );
//	case ID_IMG_TB_OPEN_TIN              : return( svg_tb_open_tin );
	case ID_IMG_TB_SAVE                  : return( svg_tb_save );
//	case ID_IMG_TB_SCATTERPLOT_PARAMETERS: return( svg_tb_scatterplot_parameters );
	case ID_IMG_TB_SHOW                  : return( svg_tb_show );
//	case ID_IMG_TB_TABLE_COL_ADD         : return( svg_tb_table_col_add );
//	case ID_IMG_TB_TABLE_COL_DEL         : return( svg_tb_table_col_del );
//	case ID_IMG_TB_TABLE_ROW_ADD         : return( svg_tb_table_row_add );
//	case ID_IMG_TB_TABLE_ROW_DEL         : return( svg_tb_table_row_del );
//	case ID_IMG_TB_TABLE_ROW_DEL_ALL     : return( svg_tb_table_row_del_all );
//	case ID_IMG_TB_TABLE_ROW_INS         : return( svg_tb_table_row_ins );
//	case ID_IMG_TB_TABLE_SELECTION_ONLY  : return( svg_tb_table_selection_only );
	case ID_IMG_TB_WKSP                  : return( svg_tb_wksp );

	case ID_IMG_WKSP_DATA_MANAGER        : return( svg_wksp_data_manager );
//	case ID_IMG_WKSP_GRID_MANAGER        : return( svg_wksp_grid_manager );
//	case ID_IMG_WKSP_GRID_SYSTEM         : return( svg_wksp_grid_system );
//	case ID_IMG_WKSP_GRID                : return( svg_wksp_grid );
//	case ID_IMG_WKSP_GRIDS               : return( svg_wksp_grid_manager );
//	case ID_IMG_WKSP_MAP                 : return( svg_wksp_map );
	case ID_IMG_WKSP_MAP_GRATICULE       : return( svg_wksp_map_graticule );
//	case ID_IMG_WKSP_MAP_BASEMAP         : return( svg_wnd_map );
//	case ID_IMG_WKSP_MAP_MANAGER         : return( svg_wksp_map_manager );
	case ID_IMG_WKSP_TOOL                : return( svg_wksp_module );
	case ID_IMG_WKSP_TOOL_CHAIN          : return( svg_wksp_module );
	case ID_IMG_WKSP_TOOL_LIBRARY        : return( svg_wksp_module_library );
	case ID_IMG_WKSP_TOOL_GROUP          : return( svg_wksp_module_library );
	case ID_IMG_WKSP_TOOL_MANAGER        : return( svg_wksp_module_manager );
//	case ID_IMG_WKSP_NOITEMS             : return( svg_wksp_noitems );
	case ID_IMG_WKSP_DB_SOURCES          : return( svg_wksp_odbc_sources );
//	case ID_IMG_WKSP_DB_SOURCE_ON        : return( svg_wksp_odbc_source_on );
//	case ID_IMG_WKSP_DB_SOURCE_OFF       : return( svg_wksp_odbc_source_off );
//	case ID_IMG_WKSP_DB_TABLE            : return( svg_wksp_table );
//	case ID_IMG_WKSP_SHAPES_LINE         : return( svg_wksp_shapes_line );
	case ID_IMG_WKSP_SHAPES_MANAGER      : return( svg_wksp_shapes_manager );
//	case ID_IMG_WKSP_SHAPES_POINT        : return( svg_wksp_shapes_point );
//	case ID_IMG_WKSP_SHAPES_POINTS       : return( svg_wksp_shapes_points );
//	case ID_IMG_WKSP_SHAPES_POLYGON      : return( svg_wksp_shapes_polygon );
//	case ID_IMG_WKSP_TABLE               : return( svg_wksp_table );
//	case ID_IMG_WKSP_TABLE_MANAGER       : return( svg_wksp_table_manager );
//	case ID_IMG_WKSP_TIN                 : return( svg_wksp_tin );
//	case ID_IMG_WKSP_TIN_MANAGER         : return( svg_wksp_tin_manager );
//	case ID_IMG_WKSP_POINTCLOUD          : return( svg_wksp_shapes_points );
//	case ID_IMG_WKSP_POINTCLOUD_MANAGER  : return( svg_wksp_shapes_points );
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxBitmapBundle	IMG_Get_Bitmaps(int ID_IMG)
{
	return( IMG_Get_Bitmaps(ID_IMG, wxSize(64, 64)) );
}

wxBitmapBundle	IMG_Get_Bitmaps(int ID_IMG, const wxSize &Size)
{
	const char *SVG = _Get_SVG(ID_IMG);

	if( SVG )
	{
		return( wxBitmapBundle::FromSVG(SVG, Size) );
	}

	return( wxBitmap(IMG_Get_Image(ID_IMG, Size)) );
}

//---------------------------------------------------------
wxBitmap		IMG_Get_Bitmap(int ID_IMG)
{
	return( wxBitmap(IMG_Get_Image(ID_IMG)) );
}

wxBitmap		IMG_Get_Bitmap(int ID_IMG, const wxSize &Size)
{
	return( IMG_Get_Bitmaps(ID_IMG).GetBitmap(Size) );
}

//---------------------------------------------------------
wxIconBundle	IMG_Get_Icons(int ID_IMG)
{
	const char *SVG = _Get_SVG(ID_IMG);

	if( SVG )
	{
		static const int Sizes[10] = { 16, 24, 32, 48, 64, 72, 80, 96, 128, 256 }; // either 16x16 or 32x32 must be provided for MSW!

		wxIconBundle Icons; wxBitmapBundle Bitmaps = wxBitmapBundle::FromSVG(SVG, wxSize(256, 256));

		for(int i=0; i<10; i++)
		{
			Icons.AddIcon(Bitmaps.GetIcon(wxSize(Sizes[i], Sizes[i])));
		}

		Icons.AddIcon(Bitmaps.GetIcon(wxSize(
			wxSystemSettings::GetMetric(wxSYS_ICON_X),
			wxSystemSettings::GetMetric(wxSYS_ICON_Y)))
		);

		return( Icons );
	}

	return( wxIcon(_Get_XPM(ID_IMG)) );
}

//---------------------------------------------------------
wxCursor		IMG_Get_Cursor(int ID_IMG)
{
	wxImage	Image(IMG_Get_Image(ID_IMG));

	Image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 15);
	Image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 15);

	return( wxCursor(Image) );
}

//---------------------------------------------------------
#endif // ifdef WITH_WXBMPBUNDLE


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
