/**********************************************************
 * Version $Id$
 *********************************************************/

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
//                   RES_Commands.cpp                    //
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
#include <wx/event.h>
#include <wx/menu.h>

#include <saga_api/saga_api.h>

#include "res_commands.h"
#include "res_images.h"

#include "saga_frame.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CMD_Get_Name(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	case ID_CMD_FRAME_QUIT:				return( _TL("[CMD] Exit") );

	case ID_CMD_FRAME_HELP:				return( _TL("[CMD] Help") );
	case ID_CMD_FRAME_ABOUT:			return( _TL("[CMD] About SAGA") );
	case ID_CMD_FRAME_TIPS:				return( _TL("[CMD] Tip of the Day...") );

	case ID_CMD_FRAME_CASCADE:			return( _TL("[CMD] Cascade") );
	case ID_CMD_FRAME_TILE_HORZ:		return( _TL("[CMD] Tile Horizontally") );
	case ID_CMD_FRAME_TILE_VERT:		return( _TL("[CMD] Tile Vertically") );
	case ID_CMD_FRAME_ARRANGEICONS:		return( _TL("[CMD] Arrange Icons") );
	case ID_CMD_FRAME_NEXT:				return( _TL("[CMD] Next") );
	case ID_CMD_FRAME_PREVIOUS:			return( _TL("[CMD] Previous") );
	case ID_CMD_FRAME_CLOSE:			return( _TL("[CMD] Close") );
	case ID_CMD_FRAME_CLOSE_ALL:		return( _TL("[CMD] Close All") );

	case ID_CMD_FRAME_INFO_SHOW:		return( _TL("[CMD] Show Message Window") );
	case ID_CMD_FRAME_DATA_SOURCE_SHOW:	return( _TL("[CMD] Show Data Source Window") );
	case ID_CMD_FRAME_ACTIVE_SHOW:		return( _TL("[CMD] Show Object Properties") );
	case ID_CMD_FRAME_WKSP_SHOW:		return( _TL("[CMD] Show Workspace") );


	//-----------------------------------------------------
	// Workspace...

	case ID_CMD_WKSP_ITEM_CLOSE:		return( _TL("[CMD] Close") );
	case ID_CMD_WKSP_ITEM_SHOW:			return( _TL("[CMD] Show") );
	case ID_CMD_WKSP_ITEM_SETTINGS_LOAD:return( _TL("[CMD] Load Settings") );
	case ID_CMD_WKSP_ITEM_SETTINGS_COPY:return( _TL("[CMD] Copy Settings from other Layer") );
	case ID_CMD_WKSP_ITEM_SEARCH:		return( _TL("[CMD] Search for...") );

	case ID_CMD_WKSP_OPEN:				return( _TL("[CMD] Load") );

	case ID_CMD_INFO_CLEAR:				return( _TL("[CMD] Clear") );
	case ID_CMD_INFO_COPY:				return( _TL("[CMD] Copy") );

	case ID_CMD_DATASOURCE_REFRESH:		return( _TL("[CMD] Refresh") );

	case ID_CMD_MODULES_OPEN:			return( _TL("[CMD] Load Module Library") );
	case ID_CMD_MODULES_SAVE_SCRIPT:	return( _TL("[CMD] Create Script Command File") );

	case ID_CMD_DATA_PROJECT_NEW:		return( _TL("[CMD] New Project") );
	case ID_CMD_DATA_PROJECT_OPEN:		return( _TL("[CMD] Load Project") );
	case ID_CMD_DATA_PROJECT_OPEN_ADD:	return( _TL("[CMD] Add Project") );
	case ID_CMD_DATA_PROJECT_SAVE:		return( _TL("[CMD] Save Project") );
	case ID_CMD_DATA_PROJECT_SAVE_AS:	return( _TL("[CMD] Save Project As...") );
	case ID_CMD_DATA_LEGEND_COPY:		return( _TL("[CMD] Copy Legend to Clipboard...") );
	case ID_CMD_DATA_LEGEND_SIZE_INC:	return( _TL("[CMD] Increase Legend Size") );
	case ID_CMD_DATA_LEGEND_SIZE_DEC:	return( _TL("[CMD] Decrease Legend Size") );
	case ID_CMD_DATA_HISTORY_CLEAR:		return( _TL("[CMD] Clear History") );

	case ID_CMD_GRIDS_OPEN:
	case ID_CMD_SHAPES_OPEN:
	case ID_CMD_TABLES_OPEN:
	case ID_CMD_TIN_OPEN:
	case ID_CMD_POINTCLOUD_OPEN:		return( _TL("[CMD] Load") );

	case ID_CMD_GRIDS_SAVE:
	case ID_CMD_SHAPES_SAVE:
	case ID_CMD_TABLES_SAVE:
	case ID_CMD_TIN_SAVE:
	case ID_CMD_POINTCLOUD_SAVE:		return( _TL("[CMD] Save") );

	case ID_CMD_GRIDS_SAVEAS:
	case ID_CMD_SHAPES_SAVEAS:
	case ID_CMD_TABLES_SAVEAS:
	case ID_CMD_TIN_SAVEAS:
	case ID_CMD_POINTCLOUD_SAVEAS:		return( _TL("[CMD] Save As...") );

	case ID_CMD_GRIDS_SHOW:
	case ID_CMD_SHAPES_SHOW:
	case ID_CMD_TIN_SHOW:
	case ID_CMD_POINTCLOUD_SHOW:		return( _TL("[CMD] Add to Map") );

	case ID_CMD_TABLES_SHOW:			return( _TL("[CMD] Show") );

	case ID_CMD_GRIDS_SET_LUT:
	case ID_CMD_SHAPES_SET_LUT:			return( _TL("[CMD] Create Lookup Table") );

	case ID_CMD_GRIDS_HISTOGRAM:
	case ID_CMD_SHAPES_HISTOGRAM:		return( _TL("[CMD] Histogram") );

	case ID_CMD_GRIDS_SCATTERPLOT:
	case ID_CMD_TABLES_SCATTERPLOT:		return( _TL("[CMD] Scatterplot") );

	case ID_CMD_TABLES_DIAGRAM:			return( _TL("[CMD] Diagram") );

	case ID_CMD_GRIDS_SAVEAS_IMAGE:		return( _TL("[CMD] Save As Image...") );
	case ID_CMD_GRIDS_EQUALINTERVALS:	return( _TL("[CMD] Create Normalised Classification") );
	case ID_CMD_GRIDS_RANGE_MINMAX:		return( _TL("[CMD] Set Range to Minmum/Maximum") );
	case ID_CMD_GRIDS_RANGE_STDDEV150:	return( _TL("[CMD] Set Range to Standard Deviation (1.5)") );
	case ID_CMD_GRIDS_RANGE_STDDEV200:	return( _TL("[CMD] Set Range to Standard Deviation (2.0)") );

	case ID_CMD_SHAPES_EDIT_SHAPE:		return( _TL("[CMD] Edit Selected Shape") );
	case ID_CMD_SHAPES_EDIT_SEL_INVERT:	return( _TL("[CMD] Invert Selection") );
	case ID_CMD_SHAPES_EDIT_ADD_SHAPE:	return( _TL("[CMD] Add Shape") );
	case ID_CMD_SHAPES_EDIT_ADD_PART:	return( _TL("[CMD] Add Part") );
	case ID_CMD_SHAPES_EDIT_DEL_SHAPE:	return( _TL("[CMD] Delete Selected Shape(s)") );
	case ID_CMD_SHAPES_EDIT_DEL_PART:	return( _TL("[CMD] Delete Selected Part") );
	case ID_CMD_SHAPES_EDIT_DEL_POINT:	return( _TL("[CMD] Delete Selected Point") );

	case ID_CMD_POINTCLOUD_RANGE_MINMAX:		return( _TL("[CMD] Set Range to Minmum/Maximum") );
	case ID_CMD_POINTCLOUD_RANGE_STDDEV150:		return( _TL("[CMD] Set Range to Standard Deviation (1.5)") );
	case ID_CMD_POINTCLOUD_RANGE_STDDEV200:		return( _TL("[CMD] Set Range to Standard Deviation (2.0)") );

	case ID_CMD_MAP_3D_SHOW:
	case ID_CMD_MAPS_3D_SHOW:					return( _TL("[CMD] Show 3D-View") );
	case ID_CMD_MAP_LAYOUT_SHOW:
	case ID_CMD_MAPS_LAYOUT_SHOW:				return( _TL("[CMD] Show Print Layout") );
	case ID_CMD_MAP_SAVE_IMAGE_ON_CHANGE:
	case ID_CMD_MAPS_SAVE_IMAGE_ON_CHANGE:		return( _TL("[CMD] Save As Image when changed") );
	case ID_CMD_MAPS_SAVE_IMAGE_TO_MEMORY:		return( _TL("[CMD] Save As Image to Workspace") );
	case ID_CMD_MAP_SAVE_IMAGE:
	case ID_CMD_MAPS_SAVE_IMAGE:				return( _TL("[CMD] Save As Image...") );
	case ID_CMD_MAPS_SAVE_TO_CLIPBOARD:			return( _TL("[CMD] Copy Map to Clipboard [Ctrl+C]") );
	case ID_CMD_MAPS_SAVE_TO_CLIPBOARD_LEGEND:	return( _TL("[CMD] Copy Legend to Clipboard [Ctrl+L]") );
	case ID_CMD_MAP_SYNCHRONIZE:
	case ID_CMD_MAPS_SYNCHRONIZE:				return( _TL("[CMD] Synchronise Map Extents") );

	case ID_CMD_MAPS_SHOW:				return( _TL("[CMD] Show Map") );
	case ID_CMD_MAPS_LAYER_SHOW:		return( _TL("[CMD] Show Layer") );
	case ID_CMD_MAPS_MOVE_TOP:			return( _TL("[CMD] Move To Top") );
	case ID_CMD_MAPS_MOVE_BOTTOM:		return( _TL("[CMD] Move To Bottom") );
	case ID_CMD_MAPS_MOVE_UP:			return( _TL("[CMD] Move Up") );
	case ID_CMD_MAPS_MOVE_DOWN:			return( _TL("[CMD] Move Down") );
	case ID_CMD_MAPS_GRID_FITCOLORS:	return( _TL("[CMD] Fit Grid Colors To Map Extent") );

	//-----------------------------------------------------
	// Views...

	case ID_CMD_MAP_MODE_ZOOM:			return( _TL("[CMD] Zoom") );
	case ID_CMD_MAP_MODE_PAN:			return( _TL("[CMD] Pan") );
	case ID_CMD_MAP_MODE_SELECT:		return( _TL("[CMD] Action") );
	case ID_CMD_MAP_MODE_DISTANCE:		return( _TL("[CMD] Measure Distance") );
	case ID_CMD_MAP_CLOSE:				return( _TL("[CMD] Close") );
	case ID_CMD_MAP_TOOLBAR:			return( _TL("[CMD] Show Toolbar") );
	case ID_CMD_MAP_LEGEND_VERTICAL:	return( _TL("[CMD] Show Vertical Legend") );
	case ID_CMD_MAP_LEGEND_HORIZONTAL:	return( _TL("[CMD] Show Horizontal Legend") );
	case ID_CMD_MAP_ZOOM_BACK:			return( _TL("[CMD] Zoom To Previous Extent") );
	case ID_CMD_MAP_ZOOM_FORWARD:		return( _TL("[CMD] Zoom To Next Extent") );
	case ID_CMD_MAP_ZOOM_FULL:			return( _TL("[CMD] Zoom To Full Extent") );
	case ID_CMD_MAP_ZOOM_ACTIVE:		return( _TL("[CMD] Zoom To Active Layer") );
	case ID_CMD_MAP_ZOOM_SELECTION:		return( _TL("[CMD] Zoom To Selection") );
	case ID_CMD_MAP_ZOOM_EXTENT:		return( _TL("[CMD] Zoom To Extent") );

	case ID_CMD_MAP3D_PARAMETERS:		return( _TL("[CMD] Properties") );
	case ID_CMD_MAP3D_ROTATE_X_LESS:	return( _TL("[CMD] Up") );
	case ID_CMD_MAP3D_ROTATE_X_MORE:	return( _TL("[CMD] Down") );
	case ID_CMD_MAP3D_ROTATE_Y_LESS:	return( _TL("[CMD] Left [F3]") );
	case ID_CMD_MAP3D_ROTATE_Y_MORE:	return( _TL("[CMD] Right [F4]") );
	case ID_CMD_MAP3D_ROTATE_Z_LESS:	return( _TL("[CMD] Left") );
	case ID_CMD_MAP3D_ROTATE_Z_MORE:	return( _TL("[CMD] Right") );
	case ID_CMD_MAP3D_SHIFT_X_LESS:		return( _TL("[CMD] Left") );
	case ID_CMD_MAP3D_SHIFT_X_MORE:		return( _TL("[CMD] Right") );
	case ID_CMD_MAP3D_SHIFT_Y_LESS:		return( _TL("[CMD] Down") );
	case ID_CMD_MAP3D_SHIFT_Y_MORE:		return( _TL("[CMD] Up") );
	case ID_CMD_MAP3D_SHIFT_Z_LESS:		return( _TL("[CMD] Forward") );
	case ID_CMD_MAP3D_SHIFT_Z_MORE:		return( _TL("[CMD] Backward") );
	case ID_CMD_MAP3D_EXAGGERATE_LESS:	return( _TL("[CMD] Decrease Exaggeration [F1]") );
	case ID_CMD_MAP3D_EXAGGERATE_MORE:	return( _TL("[CMD] Increase Exaggeration [F2]") );
	case ID_CMD_MAP3D_CENTRAL_LESS:		return( _TL("[CMD] Decrease Perspectivic Distance [F5]") );
	case ID_CMD_MAP3D_CENTRAL_MORE:		return( _TL("[CMD] Increase Perspectivic Distance [F6]") );
	case ID_CMD_MAP3D_CENTRAL:			return( _TL("[CMD] Central Projection") );
	case ID_CMD_MAP3D_STEREO_LESS:		return( _TL("[CMD] Decrease Eye Distance") );
	case ID_CMD_MAP3D_STEREO_MORE:		return( _TL("[CMD] Increase Eye Distance") );
	case ID_CMD_MAP3D_STEREO:			return( _TL("[CMD] Anaglyph [S]") );
	case ID_CMD_MAP3D_INTERPOLATED:		return( _TL("[CMD] Interpolated Colors") );
	case ID_CMD_MAP3D_SAVE:				return( _TL("[CMD] Save As Image...") );
	case ID_CMD_MAP3D_SEQ_POS_ADD:		return( _TL("[CMD] Add Position [A]") );
	case ID_CMD_MAP3D_SEQ_POS_DEL:		return( _TL("[CMD] Delete Last Position [D]") );
	case ID_CMD_MAP3D_SEQ_POS_DEL_ALL:	return( _TL("[CMD] Delete All Positions") );
	case ID_CMD_MAP3D_SEQ_POS_EDIT:		return( _TL("[CMD] Edit Positions") );
	case ID_CMD_MAP3D_SEQ_PLAY:			return( _TL("[CMD] Play Once [P]") );
	case ID_CMD_MAP3D_SEQ_PLAY_LOOP:	return( _TL("[CMD] Play Loop [L]") );
	case ID_CMD_MAP3D_SEQ_SAVE:			return( _TL("[CMD] Play And Save As Images...") );

	case ID_CMD_LAYOUT_PRINT_SETUP:		return( _TL("[CMD] Print Setup") );
	case ID_CMD_LAYOUT_PAGE_SETUP:		return( _TL("[CMD] Page Setup") );
	case ID_CMD_LAYOUT_PRINT:			return( _TL("[CMD] Print") );
	case ID_CMD_LAYOUT_PRINT_PREVIEW:	return( _TL("[CMD] Print Preview") );
	case ID_CMD_LAYOUT_FIT_SCALE:		return( _TL("[CMD] Fit Scale...") );

	case ID_CMD_TABLE_FIELD_ADD:		return( _TL("[CMD] Add Field") );
	case ID_CMD_TABLE_FIELD_DEL:		return( _TL("[CMD] Delete Fields") );
	case ID_CMD_TABLE_FIELD_SORT:		return( _TL("[CMD] Sort Fields") );
	case ID_CMD_TABLE_FIELD_RENAME:		return( _TL("[CMD] Rename Fields") );
	case ID_CMD_TABLE_RECORD_ADD:		return( _TL("[CMD] Add Record") );
	case ID_CMD_TABLE_RECORD_INS:		return( _TL("[CMD] Insert Record") );
	case ID_CMD_TABLE_RECORD_DEL:		return( _TL("[CMD] Delete Selected Record(s)") );
	case ID_CMD_TABLE_RECORD_DEL_ALL:	return( _TL("[CMD] Delete All Records") );
	case ID_CMD_TABLE_SELECTION_TO_TOP:	return( _TL("[CMD] Sort Selection to Top") );
	case ID_CMD_TABLE_AUTOSIZE_COLS:	return( _TL("[CMD] Fit Column Sizes") );
	case ID_CMD_TABLE_AUTOSIZE_ROWS:	return( _TL("[CMD] Fit Row Sizes") );

	case ID_CMD_DIAGRAM_PARAMETERS:		return( _TL("[CMD] Properties") );
	case ID_CMD_DIAGRAM_SIZE_FIT:		return( _TL("[CMD] Fit Diagram to Window") );
	case ID_CMD_DIAGRAM_SIZE_INC:		return( _TL("[CMD] Increase Diagram Size") );
	case ID_CMD_DIAGRAM_SIZE_DEC:		return( _TL("[CMD] Decrease Diagram Size") );

	case ID_CMD_SCATTERPLOT_PARAMETERS:	return( _TL("[CMD] Properties") );
	case ID_CMD_SCATTERPLOT_UPDATE:		return( _TL("[CMD] Update Data") );

	case ID_CMD_HISTOGRAM_CUMULATIVE:	return( _TL("[CMD] Cumulative") );
	case ID_CMD_HISTOGRAM_AS_TABLE:		return( _TL("[CMD] Convert To Table") );
	}

	//-----------------------------------------------------
	return( _TL("[CMD] UNRECOGNISED COMMAND ID") );
}

//---------------------------------------------------------
wxString CMD_Get_Help(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	case ID_CMD_FRAME_QUIT:				return( _TL("[CMD] Exit SAGA") );
	case ID_CMD_FRAME_ABOUT:			return( _TL("[CMD] About SAGA") );
	}

	return( CMD_Get_Name(Cmd_ID) );
}

//---------------------------------------------------------
int CMD_Get_ImageID(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	case ID_CMD_FRAME_WKSP_SHOW:		return( ID_IMG_TB_WKSP );
	case ID_CMD_FRAME_DATA_SOURCE_SHOW:	return( ID_IMG_TB_ACTIVE );
	case ID_CMD_FRAME_ACTIVE_SHOW:		return( ID_IMG_TB_ACTIVE );
	case ID_CMD_FRAME_INFO_SHOW:		return( ID_IMG_TB_INFO );
	case ID_CMD_FRAME_HELP:				return( ID_IMG_TB_HELP );
	case ID_CMD_WKSP_OPEN:				return( ID_IMG_TB_OPEN );
	case ID_CMD_MODULES_OPEN:			return( ID_IMG_TB_OPEN_MODULE );
	case ID_CMD_TABLES_OPEN:			return( ID_IMG_TB_OPEN_TABLE );
	case ID_CMD_SHAPES_OPEN:			return( ID_IMG_TB_OPEN_SHAPES );
	case ID_CMD_GRIDS_OPEN:				return( ID_IMG_TB_OPEN_GRID );
	case ID_CMD_TIN_OPEN:				return( ID_IMG_TB_OPEN_TIN );
	case ID_CMD_POINTCLOUD_OPEN:		return( ID_IMG_TB_OPEN_POINTCLOUD );

	case ID_CMD_MAP_MODE_DISTANCE:		return( ID_IMG_TB_MAP_MODE_DISTANCE );
	case ID_CMD_MAP_MODE_SELECT:		return( ID_IMG_TB_MAP_MODE_SELECT );
	case ID_CMD_MAP_MODE_ZOOM:			return( ID_IMG_TB_MAP_MODE_ZOOM );
	case ID_CMD_MAP_MODE_PAN:			return( ID_IMG_TB_MAP_MODE_PAN );
	case ID_CMD_MAP_ZOOM_FULL:			return( ID_IMG_TB_MAP_ZOOM_FULL );
	case ID_CMD_MAP_ZOOM_BACK:			return( ID_IMG_TB_MAP_ZOOM_LAST );
	case ID_CMD_MAP_ZOOM_FORWARD:		return( ID_IMG_TB_MAP_ZOOM_NEXT );
	case ID_CMD_MAP_ZOOM_ACTIVE:		return( ID_IMG_TB_MAP_ZOOM_ACTIVE );
	case ID_CMD_MAP_ZOOM_SELECTION:		return( ID_IMG_TB_MAP_ZOOM_SELECTION );
	case ID_CMD_MAP_ZOOM_EXTENT:		return( ID_IMG_TB_MAP_ZOOM_SELECTION );
	case ID_CMD_MAP_3D_SHOW:			return( ID_IMG_TB_MAP_3D_SHOW );
	case ID_CMD_MAPS_3D_SHOW:			return( ID_IMG_TB_MAP_3D_SHOW );
	case ID_CMD_MAP_LAYOUT_SHOW:		return( ID_IMG_TB_MAP_LAYOUT_SHOW );
	case ID_CMD_MAPS_LAYOUT_SHOW:		return( ID_IMG_TB_MAP_LAYOUT_SHOW );
	case ID_CMD_MAPS_SAVE_TO_CLIPBOARD:			break;
	case ID_CMD_MAPS_SAVE_TO_CLIPBOARD_LEGEND:	break;
	case ID_CMD_MAPS_SAVE_IMAGE:				break;
	case ID_CMD_MAPS_SYNCHRONIZE:		return( ID_IMG_TB_MAP_SYNCHRONIZE );
	case ID_CMD_MAP_LEGEND_VERTICAL:	return( ID_IMG_TB_MAP_LEGEND_VERTICAL );
	case ID_CMD_MAP_LEGEND_HORIZONTAL:	return( ID_IMG_TB_MAP_LEGEND_HORIZONTAL );

	case ID_CMD_MAP3D_PARAMETERS:		return( ID_IMG_TB_MAP3D_PROPERTIES );
	case ID_CMD_MAP3D_CENTRAL_LESS:		return( ID_IMG_TB_MAP3D_CENTRAL_LESS );
	case ID_CMD_MAP3D_CENTRAL_MORE:		return( ID_IMG_TB_MAP3D_CENTRAL_MORE );
	case ID_CMD_MAP3D_EXAGGERATE_LESS:	return( ID_IMG_TB_MAP3D_EXAGGERATE_LESS );
	case ID_CMD_MAP3D_EXAGGERATE_MORE:	return( ID_IMG_TB_MAP3D_EXAGGERATE_MORE );
	case ID_CMD_MAP3D_INTERPOLATED:		return( ID_IMG_TB_MAP3D_INTERPOLATED );
	case ID_CMD_MAP3D_ROTATE_X_LESS:	return( ID_IMG_TB_MAP3D_ROTATE_X_LESS );
	case ID_CMD_MAP3D_ROTATE_X_MORE:	return( ID_IMG_TB_MAP3D_ROTATE_X_MORE );
	case ID_CMD_MAP3D_ROTATE_Z_LESS:	return( ID_IMG_TB_MAP3D_ROTATE_Z_LESS );
	case ID_CMD_MAP3D_ROTATE_Z_MORE:	return( ID_IMG_TB_MAP3D_ROTATE_Z_MORE );
	case ID_CMD_MAP3D_SHIFT_X_LESS:		return( ID_IMG_TB_MAP3D_SHIFT_X_LESS );
	case ID_CMD_MAP3D_SHIFT_X_MORE:		return( ID_IMG_TB_MAP3D_SHIFT_X_MORE );
	case ID_CMD_MAP3D_SHIFT_Y_LESS:		return( ID_IMG_TB_MAP3D_SHIFT_Y_LESS );
	case ID_CMD_MAP3D_SHIFT_Y_MORE:		return( ID_IMG_TB_MAP3D_SHIFT_Y_MORE );
	case ID_CMD_MAP3D_SHIFT_Z_LESS:		return( ID_IMG_TB_MAP3D_SHIFT_Z_LESS );
	case ID_CMD_MAP3D_SHIFT_Z_MORE:		return( ID_IMG_TB_MAP3D_SHIFT_Z_MORE );
	case ID_CMD_MAP3D_STEREO:			return( ID_IMG_TB_MAP3D_STEREO );

	case ID_CMD_LAYOUT_PRINT_SETUP:		return( ID_IMG_TB_LAYOUT_PRINT_SETUP );
	case ID_CMD_LAYOUT_PAGE_SETUP:		return( ID_IMG_TB_LAYOUT_PAGE_SETUP );
	case ID_CMD_LAYOUT_PRINT:			return( ID_IMG_TB_LAYOUT_PRINT );
	case ID_CMD_LAYOUT_PRINT_PREVIEW:	return( ID_IMG_TB_LAYOUT_PRINT_PREVIEW );

	case ID_CMD_HISTOGRAM_CUMULATIVE:	return( ID_IMG_TB_HISTOGRAM_CUMULATIVE );
	case ID_CMD_HISTOGRAM_AS_TABLE:		return( ID_IMG_TB_HISTOGRAM_AS_TABLE );

	case ID_CMD_TABLE_FIELD_ADD:		return( ID_IMG_TB_TABLE_COL_ADD );
	case ID_CMD_TABLE_FIELD_DEL:		return( ID_IMG_TB_TABLE_COL_DEL );
	case ID_CMD_TABLE_RECORD_ADD:		return( ID_IMG_TB_TABLE_ROW_ADD );
	case ID_CMD_TABLE_RECORD_INS:		return( ID_IMG_TB_TABLE_ROW_INS );
	case ID_CMD_TABLE_RECORD_DEL:		return( ID_IMG_TB_TABLE_ROW_DEL );
	case ID_CMD_TABLE_RECORD_DEL_ALL:	return( ID_IMG_TB_TABLE_ROW_DEL_ALL );

	case ID_CMD_SCATTERPLOT_PARAMETERS:	return( ID_IMG_TB_SCATTERPLOT_PARAMETERS );
	case ID_CMD_SCATTERPLOT_UPDATE:		return( ID_IMG_DEFAULT );

	case ID_CMD_DIAGRAM_PARAMETERS:		return( ID_IMG_TB_DIAGRAM_PARAMETERS );
	case ID_CMD_DIAGRAM_SIZE_FIT:		break;
	case ID_CMD_DIAGRAM_SIZE_INC:		return( ID_IMG_TB_MAP3D_CENTRAL_MORE );
	case ID_CMD_DIAGRAM_SIZE_DEC:		return( ID_IMG_TB_MAP3D_CENTRAL_LESS );
	}

	return( ID_IMG_DEFAULT );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CMD_Menu_Add_Item(wxMenu *pMenu, bool bCheck, int Cmd_ID)
{
	if( bCheck )
	{
		pMenu->AppendCheckItem(Cmd_ID, CMD_Get_Name(Cmd_ID), CMD_Get_Help(Cmd_ID));
	}
	else
	{
		pMenu->Append         (Cmd_ID, CMD_Get_Name(Cmd_ID), CMD_Get_Help(Cmd_ID));
	}
}

//---------------------------------------------------------
void CMD_Menu_Ins_Item(wxMenu *pMenu, bool bCheck, int Cmd_ID, int Position)
{
	if( bCheck )
	{
		pMenu->InsertCheckItem(Position, Cmd_ID, CMD_Get_Name(Cmd_ID), CMD_Get_Help(Cmd_ID));
	}
	else
	{
		pMenu->Insert         (Position, Cmd_ID, CMD_Get_Name(Cmd_ID), CMD_Get_Help(Cmd_ID));
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxToolBarBase * CMD_ToolBar_Create(int ID)
{
	return( g_pSAGA_Frame ? g_pSAGA_Frame->TB_Create(ID) : NULL );
}

//---------------------------------------------------------
void CMD_ToolBar_Add(wxToolBarBase *pToolBar, const wxString &Name)
{
	if( g_pSAGA_Frame )
	{
		g_pSAGA_Frame->TB_Add(pToolBar, Name);
	}
}

//---------------------------------------------------------
void CMD_ToolBar_Add_Item(wxToolBarBase *pToolBar, bool bCheck, int Cmd_ID)
{
	if( g_pSAGA_Frame )
	{
		g_pSAGA_Frame->TB_Add_Item(pToolBar, bCheck, Cmd_ID);
	}
}

//---------------------------------------------------------
void CMD_ToolBar_Add_Separator(wxToolBarBase *pToolBar)
{
	if( g_pSAGA_Frame )
	{
		g_pSAGA_Frame->TB_Add_Separator(pToolBar);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
