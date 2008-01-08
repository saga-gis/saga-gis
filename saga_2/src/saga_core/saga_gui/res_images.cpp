
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
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
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
#include "./res/xpm/nb_active_attributes.xpm"
#include "./res/xpm/nb_active_description.xpm"
#include "./res/xpm/nb_active_legend.xpm"
#include "./res/xpm/nb_active_parameters.xpm"
#include "./res/xpm/nb_active_htmlextrainfo.xpm"
#include "./res/xpm/nb_info_error.xpm"
#include "./res/xpm/nb_info_execution.xpm"
#include "./res/xpm/nb_info_messages.xpm"
#include "./res/xpm/nb_wksp_data.xpm"
#include "./res/xpm/nb_wksp_maps.xpm"
#include "./res/xpm/nb_wksp_modules.xpm"
#include "./res/xpm/nb_wksp_thumbnails.xpm"
#include "./res/xpm/nb_wksp_treeview.xpm"
#include "./res/xpm/saga_icon_16.xpm"
#include "./res/xpm/saga_icon_32.xpm"
#include "./res/xpm/saga_splash.xpm"
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
#include "./res/xpm/tb_info.xpm"
#include "./res/xpm/tb_layout_page_setup.xpm"
#include "./res/xpm/tb_layout_print.xpm"
#include "./res/xpm/tb_layout_print_preview.xpm"
#include "./res/xpm/tb_layout_print_setup.xpm"
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
#include "./res/xpm/tb_map_synchronize.xpm"
#include "./res/xpm/tb_map_zoom_active.xpm"
#include "./res/xpm/tb_map_zoom_full.xpm"
#include "./res/xpm/tb_map_zoom_last.xpm"
#include "./res/xpm/tb_map_zoom_next.xpm"
#include "./res/xpm/tb_map_zoom_selection.xpm"
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
#include "./res/xpm/tb_wksp.xpm"

#include "./res/xpm/wksp_data_manager.xpm"
#include "./res/xpm/wksp_grid.xpm"
#include "./res/xpm/wksp_grid_manager.xpm"
#include "./res/xpm/wksp_grid_system.xpm"
#include "./res/xpm/wksp_map.xpm"
#include "./res/xpm/wksp_map_manager.xpm"
#include "./res/xpm/wksp_module.xpm"
#include "./res/xpm/wksp_module_library.xpm"
#include "./res/xpm/wksp_module_manager.xpm"
#include "./res/xpm/wksp_noitems.xpm"
#include "./res/xpm/wksp_shapes_line.xpm"
#include "./res/xpm/wksp_shapes_manager.xpm"
#include "./res/xpm/wksp_shapes_point.xpm"
#include "./res/xpm/wksp_shapes_points.xpm"
#include "./res/xpm/wksp_shapes_polygon.xpm"
#include "./res/xpm/wksp_table.xpm"
#include "./res/xpm/wksp_table_manager.xpm"
#include "./res/xpm/wksp_tin.xpm"
#include "./res/xpm/wksp_tin_manager.xpm"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
char ** IMG_Get_XPM(int ID_IMG)
{
	switch( ID_IMG )
	{
	default:
	case ID_IMG_DEFAULT:	return( default_xpm );
	case ID_IMG_NB_ACTIVE_ATTRIBUTES:	return( nb_active_attributes_xpm );
	case ID_IMG_NB_ACTIVE_DESCRIPTION:	return( nb_active_description_xpm );
	case ID_IMG_NB_ACTIVE_LEGEND:	return( nb_active_legend_xpm );
	case ID_IMG_NB_ACTIVE_PARAMETERS:	return( nb_active_parameters_xpm );
	case ID_IMG_NB_ACTIVE_HTMLEXTRAINFO:	return( nb_active_htmlextrainfo_xpm );
	case ID_IMG_NB_INFO_ERROR:	return( nb_info_error_xpm );
	case ID_IMG_NB_INFO_EXECUTION:	return( nb_info_execution_xpm );
	case ID_IMG_NB_INFO_MESSAGES:	return( nb_info_messages_xpm );
	case ID_IMG_NB_WKSP_DATA:	return( nb_wksp_data_xpm );
	case ID_IMG_NB_WKSP_MAPS:	return( nb_wksp_maps_xpm );
	case ID_IMG_NB_WKSP_MODULES:	return( nb_wksp_modules_xpm );
	case ID_IMG_NB_WKSP_THUMBNAILS:	return( nb_wksp_thumbnails_xpm );
	case ID_IMG_NB_WKSP_TREEVIEW:	return( nb_wksp_treeview_xpm );
	case ID_IMG_SAGA_ICON_16:	return( saga_icon_16_xpm );
	case ID_IMG_SAGA_ICON_32:	return( saga_icon_32_xpm );
	case ID_IMG_SAGA_SPLASH:	return( saga_splash_xpm );
	case ID_IMG_WND_DIAGRAM:	return( wnd_diagram_xpm );
	case ID_IMG_WND_HISTOGRAM:	return( wnd_histogram_xpm );
	case ID_IMG_WND_LAYOUT:	return( wnd_layout_xpm );
	case ID_IMG_WND_MAP:	return( wnd_map_xpm );
	case ID_IMG_WND_MAP3D:	return( wnd_map3d_xpm );
	case ID_IMG_WND_SCATTERPLOT:	return( wnd_scatterplot_xpm );
	case ID_IMG_WND_TABLE:	return( wnd_table_xpm );

	case ID_IMG_CRS_EDIT_POINT_ADD:	return( crs_edit_point_add_xpm );
	case ID_IMG_CRS_EDIT_POINT_MOVE:	return( crs_edit_point_move_xpm );
	case ID_IMG_CRS_HAND:	return( crs_hand_xpm );
	case ID_IMG_CRS_HAND_GRAP:	return( crs_hand_grap_xpm );
	case ID_IMG_CRS_INFO:	return( crs_info_xpm );
	case ID_IMG_CRS_MAGNIFIER:	return( crs_magnifier_xpm );
	case ID_IMG_CRS_SELECT:	return( crs_select_xpm );

	case ID_IMG_TB_ACTIVE:	return( tb_active_xpm );
	case ID_IMG_TB_DELETE:	return( tb_delete_xpm );
	case ID_IMG_TB_DIAGRAM_PARAMETERS:	return( tb_diagram_parameters_xpm );
	case ID_IMG_TB_EDIT_SHAPE_ADD:	return( tb_edit_shape_add_xpm );
	case ID_IMG_TB_EDIT_SHAPE_ADD_PART:	return( tb_edit_shape_add_part_xpm );
	case ID_IMG_TB_EDIT_SHAPE_DEL:	return( tb_edit_shape_del_xpm );
	case ID_IMG_TB_EDIT_SHAPE_DEL_PART:	return( tb_edit_shape_del_part_xpm );
	case ID_IMG_TB_EDIT_SHAPE_DEL_POINT:	return( tb_edit_shape_del_point_xpm );
	case ID_IMG_TB_EDIT_SHAPE_SELECT:	return( tb_edit_shape_select_xpm );
	case ID_IMG_TB_HELP:	return( tb_help_xpm );
	case ID_IMG_TB_HISTOGRAM_AS_TABLE:	return( tb_histogram_as_table_xpm );
	case ID_IMG_TB_HISTOGRAM_CUMULATIVE:	return( tb_histogram_cumulative_xpm );
	case ID_IMG_TB_INFO:	return( tb_info_xpm );
	case ID_IMG_TB_LAYOUT_PAGE_SETUP:	return( tb_layout_page_setup_xpm );
	case ID_IMG_TB_LAYOUT_PRINT:	return( tb_layout_print_xpm );
	case ID_IMG_TB_LAYOUT_PRINT_PREVIEW:	return( tb_layout_print_preview_xpm );
	case ID_IMG_TB_LAYOUT_PRINT_SETUP:	return( tb_layout_print_setup_xpm );
	case ID_IMG_TB_MAP3D_CENTRAL_LESS:	return( tb_map3d_central_less_xpm );
	case ID_IMG_TB_MAP3D_CENTRAL_MORE:	return( tb_map3d_central_more_xpm );
	case ID_IMG_TB_MAP3D_EXAGGERATE_LESS:	return( tb_map3d_exaggerate_less_xpm );
	case ID_IMG_TB_MAP3D_EXAGGERATE_MORE:	return( tb_map3d_exaggerate_more_xpm );
	case ID_IMG_TB_MAP3D_INTERPOLATED:	return( tb_map3d_interpolated_xpm );
	case ID_IMG_TB_MAP3D_PROPERTIES:	return( tb_map3d_properties_xpm );
	case ID_IMG_TB_MAP3D_ROTATE_X_LESS:	return( tb_map3d_rotate_x_less_xpm );
	case ID_IMG_TB_MAP3D_ROTATE_X_MORE:	return( tb_map3d_rotate_x_more_xpm );
	case ID_IMG_TB_MAP3D_ROTATE_Z_LESS:	return( tb_map3d_rotate_z_less_xpm );
	case ID_IMG_TB_MAP3D_ROTATE_Z_MORE:	return( tb_map3d_rotate_z_more_xpm );
	case ID_IMG_TB_MAP3D_SHIFT_X_LESS:	return( tb_map3d_shift_x_less_xpm );
	case ID_IMG_TB_MAP3D_SHIFT_X_MORE:	return( tb_map3d_shift_x_more_xpm );
	case ID_IMG_TB_MAP3D_SHIFT_Y_LESS:	return( tb_map3d_shift_y_less_xpm );
	case ID_IMG_TB_MAP3D_SHIFT_Y_MORE:	return( tb_map3d_shift_y_more_xpm );
	case ID_IMG_TB_MAP3D_SHIFT_Z_LESS:	return( tb_map3d_shift_z_less_xpm );
	case ID_IMG_TB_MAP3D_SHIFT_Z_MORE:	return( tb_map3d_shift_z_more_xpm );
	case ID_IMG_TB_MAP3D_STEREO:	return( tb_map3d_stereo_xpm );
	case ID_IMG_TB_MAP_3D_SHOW:	return( tb_map_3d_show_xpm );
	case ID_IMG_TB_MAP_LAYOUT_SHOW:	return( tb_map_layout_show_xpm );
	case ID_IMG_TB_MAP_LEGEND_HORIZONTAL:	return( tb_map_legend_horizontal_xpm );
	case ID_IMG_TB_MAP_LEGEND_VERTICAL:	return( tb_map_legend_vertical_xpm );
	case ID_IMG_TB_MAP_MODE_DISTANCE:	return( tb_map_mode_distance_xpm );
	case ID_IMG_TB_MAP_MODE_PAN:	return( tb_map_mode_pan_xpm );
	case ID_IMG_TB_MAP_MODE_SELECT:	return( tb_map_mode_select_xpm );
	case ID_IMG_TB_MAP_MODE_ZOOM:	return( tb_map_mode_zoom_xpm );
	case ID_IMG_TB_MAP_SYNCHRONIZE:	return( tb_map_synchronize_xpm );
	case ID_IMG_TB_MAP_ZOOM_ACTIVE:	return( tb_map_zoom_active_xpm );
	case ID_IMG_TB_MAP_ZOOM_FULL:	return( tb_map_zoom_full_xpm );
	case ID_IMG_TB_MAP_ZOOM_LAST:	return( tb_map_zoom_last_xpm );
	case ID_IMG_TB_MAP_ZOOM_NEXT:	return( tb_map_zoom_next_xpm );
	case ID_IMG_TB_MAP_ZOOM_SELECTION:	return( tb_map_zoom_selection_xpm );
	case ID_IMG_TB_OPEN:	return( tb_open_xpm );
	case ID_IMG_TB_OPEN_GRID:	return( tb_open_grid_xpm );
	case ID_IMG_TB_OPEN_MODULE:	return( tb_open_module_xpm );
	case ID_IMG_TB_OPEN_SHAPES:	return( tb_open_shapes_xpm );
	case ID_IMG_TB_OPEN_TABLE:	return( tb_open_table_xpm );
	case ID_IMG_TB_OPEN_TIN:	return( tb_open_tin_xpm );
	case ID_IMG_TB_SAVE:	return( tb_save_xpm );
	case ID_IMG_TB_SCATTERPLOT_PARAMETERS:	return( tb_scatterplot_parameters_xpm );
	case ID_IMG_TB_SHOW:	return( tb_show_xpm );
	case ID_IMG_TB_TABLE_COL_ADD:	return( tb_table_col_add_xpm );
	case ID_IMG_TB_TABLE_COL_DEL:	return( tb_table_col_del_xpm );
	case ID_IMG_TB_TABLE_ROW_ADD:	return( tb_table_row_add_xpm );
	case ID_IMG_TB_TABLE_ROW_DEL:	return( tb_table_row_del_xpm );
	case ID_IMG_TB_TABLE_ROW_DEL_ALL:	return( tb_table_row_del_all_xpm );
	case ID_IMG_TB_TABLE_ROW_INS:	return( tb_table_row_ins_xpm );
	case ID_IMG_TB_WKSP:	return( tb_wksp_xpm );

	case ID_IMG_WKSP_DATA_MANAGER:	return( wksp_data_manager_xpm );
	case ID_IMG_WKSP_GRID:	return( wksp_grid_xpm );
	case ID_IMG_WKSP_GRID_MANAGER:	return( wksp_grid_manager_xpm );
	case ID_IMG_WKSP_GRID_SYSTEM:	return( wksp_grid_system_xpm );
	case ID_IMG_WKSP_MAP:	return( wksp_map_xpm );
	case ID_IMG_WKSP_MAP_MANAGER:	return( wksp_map_manager_xpm );
	case ID_IMG_WKSP_MODULE:	return( wksp_module_xpm );
	case ID_IMG_WKSP_MODULE_LIBRARY:	return( wksp_module_library_xpm );
	case ID_IMG_WKSP_MODULE_MANAGER:	return( wksp_module_manager_xpm );
	case ID_IMG_WKSP_NOITEMS:	return( wksp_noitems_xpm );
	case ID_IMG_WKSP_SHAPES_LINE:	return( wksp_shapes_line_xpm );
	case ID_IMG_WKSP_SHAPES_MANAGER:	return( wksp_shapes_manager_xpm );
	case ID_IMG_WKSP_SHAPES_POINT:	return( wksp_shapes_point_xpm );
	case ID_IMG_WKSP_SHAPES_POINTS:	return( wksp_shapes_points_xpm );
	case ID_IMG_WKSP_SHAPES_POLYGON:	return( wksp_shapes_polygon_xpm );
	case ID_IMG_WKSP_TABLE:	return( wksp_table_xpm );
	case ID_IMG_WKSP_TABLE_MANAGER:	return( wksp_table_manager_xpm );
	case ID_IMG_WKSP_TIN:	return( wksp_tin_xpm );
	case ID_IMG_WKSP_TIN_MANAGER:	return( wksp_tin_manager_xpm );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxImage		IMG_Get_Image(int ID_IMG)
{
	wxBitmap	BMP(IMG_Get_XPM(ID_IMG));
	wxImage		Image(BMP.ConvertToImage());

//	wxImage	Image((const wxChar **)IMG_Get_XPM(ID_IMG));

//	Image.SetMaskColour(192, 192, 192);

	return( Image );
}

wxImage		IMG_Get_Image(int ID_IMG, int size)
{
	return( IMG_Get_Image(ID_IMG).Rescale(size, size) );
}

//---------------------------------------------------------
wxBitmap	IMG_Get_Bitmap(int ID_IMG)
{
	return( wxBitmap(IMG_Get_XPM(ID_IMG)) );
}

wxBitmap	IMG_Get_Bitmap(int ID_IMG, int size)
{
	return( wxBitmap(IMG_Get_Image(ID_IMG, size)) );
}

//---------------------------------------------------------
wxIcon		IMG_Get_Icon(int ID_IMG)
{
	return( wxIcon(IMG_Get_XPM(ID_IMG)) );
}

//---------------------------------------------------------
wxCursor	IMG_Get_Cursor(int ID_IMG)
{
	wxImage	Image(IMG_Get_Image(ID_IMG));

	Image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 15);
	Image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 15);

	return( wxCursor(Image) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
