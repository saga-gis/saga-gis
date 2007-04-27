
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
const wxChar * CMD_Get_Name(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	case ID_CMD_FRAME_QUIT:				return( LNG("[CMD] Exit") );

	case ID_CMD_FRAME_HELP:				return( LNG("[CMD] Help") );
	case ID_CMD_FRAME_ABOUT:			return( LNG("[CMD] About SAGA") );
	case ID_CMD_FRAME_TIPS:				return( LNG("[CMD] Tip of the Day...") );

	case ID_CMD_FRAME_CASCADE:			return( LNG("[CMD] Cascade") );
	case ID_CMD_FRAME_TILE_HORZ:		return( LNG("[CMD] Tile Horizontally") );
	case ID_CMD_FRAME_TILE_VERT:		return( LNG("[CMD] Tile Vertically") );
	case ID_CMD_FRAME_ARRANGEICONS:		return( LNG("[CMD] Arrange Icons") );
	case ID_CMD_FRAME_NEXT:				return( LNG("[CMD] Next") );
	case ID_CMD_FRAME_PREVIOUS:			return( LNG("[CMD] Previous") );
	case ID_CMD_FRAME_CLOSE:			return( LNG("[CMD] Close") );
	case ID_CMD_FRAME_CLOSE_ALL:		return( LNG("[CMD] Close All") );

	case ID_CMD_FRAME_INFO_SHOW:		return( LNG("[CMD] Show Message Window") );
	case ID_CMD_FRAME_WKSP_SHOW:		return( LNG("[CMD] Show Workspace") );
	case ID_CMD_FRAME_ACTIVE_SHOW:		return( LNG("[CMD] Show Object Properties") );


	//-----------------------------------------------------
	// Workspace...

	case ID_CMD_WKSP_ITEM_CLOSE:		return( LNG("[CMD] Close") );

	case ID_CMD_WKSP_OPEN:				return( LNG("[CMD] Load") );

	case ID_CMD_MODULES_OPEN:			return( LNG("[CMD] Load Module Library") );

	case ID_CMD_DATA_PROJECT_OPEN:		return( LNG("[CMD] Load Project") );
	case ID_CMD_DATA_PROJECT_OPEN_ADD:	return( LNG("[CMD] Add Project") );
	case ID_CMD_DATA_PROJECT_SAVE:		return( LNG("[CMD] Save Project") );
	case ID_CMD_DATA_PROJECT_SAVE_AS:	return( LNG("[CMD] Save Project As...") );

	case ID_CMD_GRIDS_OPEN:				return( LNG("[CMD] Load Grid") );
	case ID_CMD_GRIDS_SAVE:				return( LNG("[CMD] Save Grid") );
	case ID_CMD_GRIDS_SAVEAS:			return( LNG("[CMD] Save Grid As...") );
	case ID_CMD_GRIDS_SAVEAS_IMAGE:		return( LNG("[CMD] Save Grid As Image...") );
	case ID_CMD_GRIDS_SHOW:				return( LNG("[CMD] Show Grid") );
	case ID_CMD_GRIDS_HISTOGRAM:		return( LNG("[CMD] Show Histogram") );
	case ID_CMD_GRIDS_SCATTERPLOT:		return( LNG("[CMD] Show Scatterplot") );
	case ID_CMD_GRIDS_EQUALINTERVALS:	return( LNG("[CMD] Create Normalised Classification") );
	case ID_CMD_GRIDS_SET_LUT:			return( LNG("[CMD] Create Lookup Table") );

	case ID_CMD_SHAPES_OPEN:			return( LNG("[CMD] Load Shapes") );
	case ID_CMD_SHAPES_SAVE:			return( LNG("[CMD] Save Shapes") );
	case ID_CMD_SHAPES_SAVEAS:			return( LNG("[CMD] Save Shapes As...") );
	case ID_CMD_SHAPES_SHOW:			return( LNG("[CMD] Show Shapes") );
	case ID_CMD_SHAPES_SET_LUT:			return( LNG("[CMD] Create Lookup Table") );
	case ID_CMD_SHAPES_EDIT_SHAPE:		return( LNG("[CMD] Edit Selected Shape") );
	case ID_CMD_SHAPES_EDIT_ADD_SHAPE:	return( LNG("[CMD] Add Shape") );
	case ID_CMD_SHAPES_EDIT_ADD_PART:	return( LNG("[CMD] Add Part") );
	case ID_CMD_SHAPES_EDIT_DEL_SHAPE:	return( LNG("[CMD] Delete Selected Shape(s)") );
	case ID_CMD_SHAPES_EDIT_DEL_PART:	return( LNG("[CMD] Delete Selected Part") );
	case ID_CMD_SHAPES_EDIT_DEL_POINT:	return( LNG("[CMD] Delete Selected Point") );

	case ID_CMD_TABLES_OPEN:			return( LNG("[CMD] Load Table") );
	case ID_CMD_TABLES_SAVE:			return( LNG("[CMD] Save Table") );
	case ID_CMD_TABLES_SAVEAS:			return( LNG("[CMD] Save Table As...") );
	case ID_CMD_TABLES_SHOW:			return( LNG("[CMD] Show Table") );
	case ID_CMD_TABLES_DIAGRAM:			return( LNG("[CMD] Show Diagram") );
	case ID_CMD_TABLES_SCATTERPLOT:		return( LNG("[CMD] Show Scatterplot") );

	case ID_CMD_TIN_OPEN:				return( LNG("[CMD] Load TIN") );
	case ID_CMD_TIN_SAVE:				return( LNG("[CMD] Save TIN") );
	case ID_CMD_TIN_SAVEAS:				return( LNG("[CMD] Save TIN As...") );
	case ID_CMD_TIN_SHOW:				return( LNG("[CMD] Show TIN") );

	case ID_CMD_MAP_3D_SHOW:
	case ID_CMD_MAPS_3D_SHOW:			return( LNG("[CMD] Show 3D-View") );
	case ID_CMD_MAP_LAYOUT_SHOW:
	case ID_CMD_MAPS_LAYOUT_SHOW:		return( LNG("[CMD] Show Print Layout") );
	case ID_CMD_MAP_SAVE_IMAGE_ON_CHANGE:
	case ID_CMD_MAPS_SAVE_IMAGE_ON_CHANGE:	return( LNG("[CMD] Save As Image when changed") );
	case ID_CMD_MAP_SAVE_IMAGE:
	case ID_CMD_MAPS_SAVE_IMAGE:		return( LNG("[CMD] Save As Image") );
	case ID_CMD_MAP_SAVE_PDF_INDEXED:
	case ID_CMD_MAPS_SAVE_PDF_INDEXED:	return( LNG("[CMD] Save As PDF") );
	case ID_CMD_MAP_SAVE_INTERACTIVE_SVG:
	case ID_CMD_MAPS_SAVE_INTERACTIVE_SVG:	return( LNG("[CMD] Save as Interactive SVG") );
	case ID_CMD_MAP_SYNCHRONIZE:
	case ID_CMD_MAPS_SYNCHRONIZE:		return( LNG("[CMD] Synchronise Map Extents") );

	case ID_CMD_MAPS_SHOW:				return( LNG("[CMD] Show Map") );
	case ID_CMD_MAPS_MOVE_TOP:			return( LNG("[CMD] Move To Top") );
	case ID_CMD_MAPS_MOVE_BOTTOM:		return( LNG("[CMD] Move To Bottom") );
	case ID_CMD_MAPS_MOVE_UP:			return( LNG("[CMD] Move Up") );
	case ID_CMD_MAPS_MOVE_DOWN:			return( LNG("[CMD] Move Down") );
	case ID_CMD_MAPS_GRID_FITCOLORS:	return( LNG("[CMD] Fit Grid Colors To Map Extent") );

	//-----------------------------------------------------
	// Views...

	case ID_CMD_MAP_MODE_ZOOM:			return( LNG("[CMD] Zoom") );
	case ID_CMD_MAP_MODE_PAN:			return( LNG("[CMD] Pan") );
	case ID_CMD_MAP_MODE_SELECT:		return( LNG("[CMD] Action") );
	case ID_CMD_MAP_MODE_DISTANCE:		return( LNG("[CMD] Measure Distance") );
	case ID_CMD_MAP_CLOSE:				return( LNG("[CMD] Close") );
	case ID_CMD_MAP_TOOLBAR:			return( LNG("[CMD] Show Toolbar") );
	case ID_CMD_MAP_LEGEND_VERTICAL:	return( LNG("[CMD] Show Vertical Legend") );
	case ID_CMD_MAP_LEGEND_HORIZONTAL:	return( LNG("[CMD] Show Horizontal Legend") );
	case ID_CMD_MAP_ZOOM_LAST:			return( LNG("[CMD] Zoom To Previous Extent") );
	case ID_CMD_MAP_ZOOM_FULL:			return( LNG("[CMD] Zoom To Full Extent") );
	case ID_CMD_MAP_ZOOM_ACTIVE:		return( LNG("[CMD] Zoom To Active Layer") );
	case ID_CMD_MAP_ZOOM_SELECTION:		return( LNG("[CMD] Zoom To Selection") );
	case ID_CMD_MAP_ZOOM_EXTENT:		return( LNG("[CMD] Zoom To Extent") );

	case ID_CMD_MAP3D_PARAMETERS:		return( LNG("[CMD] Properties") );
	case ID_CMD_MAP3D_ROTATE_X_LESS:	return( LNG("[CMD] Up") );
	case ID_CMD_MAP3D_ROTATE_X_MORE:	return( LNG("[CMD] Down") );
	case ID_CMD_MAP3D_ROTATE_Y_LESS:	return( LNG("[CMD] Left [F3]") );
	case ID_CMD_MAP3D_ROTATE_Y_MORE:	return( LNG("[CMD] Right [F4]") );
	case ID_CMD_MAP3D_ROTATE_Z_LESS:	return( LNG("[CMD] Left") );
	case ID_CMD_MAP3D_ROTATE_Z_MORE:	return( LNG("[CMD] Right") );
	case ID_CMD_MAP3D_SHIFT_X_LESS:		return( LNG("[CMD] Left") );
	case ID_CMD_MAP3D_SHIFT_X_MORE:		return( LNG("[CMD] Right") );
	case ID_CMD_MAP3D_SHIFT_Y_LESS:		return( LNG("[CMD] Down") );
	case ID_CMD_MAP3D_SHIFT_Y_MORE:		return( LNG("[CMD] Up") );
	case ID_CMD_MAP3D_SHIFT_Z_LESS:		return( LNG("[CMD] Forward") );
	case ID_CMD_MAP3D_SHIFT_Z_MORE:		return( LNG("[CMD] Backward") );
	case ID_CMD_MAP3D_EXAGGERATE_LESS:	return( LNG("[CMD] Decrease Exaggeration [F1]") );
	case ID_CMD_MAP3D_EXAGGERATE_MORE:	return( LNG("[CMD] Increase Exaggeration [F2]") );
	case ID_CMD_MAP3D_CENTRAL_LESS:		return( LNG("[CMD] Decrease Perspectivic Distance [F5]") );
	case ID_CMD_MAP3D_CENTRAL_MORE:		return( LNG("[CMD] Increase Perspectivic Distance [F6]") );
	case ID_CMD_MAP3D_CENTRAL:			return( LNG("[CMD] Central Projection") );
	case ID_CMD_MAP3D_STEREO_LESS:		return( LNG("[CMD] Decrease Eye Distance") );
	case ID_CMD_MAP3D_STEREO_MORE:		return( LNG("[CMD] Increase Eye Distance") );
	case ID_CMD_MAP3D_STEREO:			return( LNG("[CMD] Anaglyph [S]") );
	case ID_CMD_MAP3D_INTERPOLATED:		return( LNG("[CMD] Interpolated Colors") );
	case ID_CMD_MAP3D_SAVE:				return( LNG("[CMD] Save As Image...") );
	case ID_CMD_MAP3D_SEQ_POS_ADD:		return( LNG("[CMD] Add Position [A]") );
	case ID_CMD_MAP3D_SEQ_POS_DEL:		return( LNG("[CMD] Delete Last Position [D]") );
	case ID_CMD_MAP3D_SEQ_POS_DEL_ALL:	return( LNG("[CMD] Delete All Positions") );
	case ID_CMD_MAP3D_SEQ_POS_EDIT:		return( LNG("[CMD] Edit Positions") );
	case ID_CMD_MAP3D_SEQ_PLAY:			return( LNG("[CMD] Play Once [P]") );
	case ID_CMD_MAP3D_SEQ_PLAY_LOOP:	return( LNG("[CMD] Play Loop [L]") );
	case ID_CMD_MAP3D_SEQ_SAVE:			return( LNG("[CMD] Play And Save As Images...") );

	case ID_CMD_LAYOUT_PRINT_SETUP:		return( LNG("[CMD] Print Setup") );
	case ID_CMD_LAYOUT_PAGE_SETUP:		return( LNG("[CMD] Page Setup") );
	case ID_CMD_LAYOUT_PRINT:			return( LNG("[CMD] Print") );
	case ID_CMD_LAYOUT_PRINT_PREVIEW:	return( LNG("[CMD] Print Preview") );
	case ID_CMD_LAYOUT_FIT_SCALE:		return( LNG("[CMD] Fit Scale...") );

	case ID_CMD_TABLE_FIELD_ADD:		return( LNG("[CMD] Add Field") );
	case ID_CMD_TABLE_FIELD_DEL:		return( LNG("[CMD] Delete Fields") );
	case ID_CMD_TABLE_FIELD_SORT:		return( LNG("[CMD] Sort Fields") );
	case ID_CMD_TABLE_RECORD_ADD:		return( LNG("[CMD] Add Record") );
	case ID_CMD_TABLE_RECORD_INS:		return( LNG("[CMD] Insert Record") );
	case ID_CMD_TABLE_RECORD_DEL:		return( LNG("[CMD] Delete Record") );
	case ID_CMD_TABLE_RECORD_DEL_ALL:	return( LNG("[CMD] Delete All Records") );
	case ID_CMD_TABLE_AUTOSIZE_COLS:	return( LNG("[CMD] Fit Column Sizes") );
	case ID_CMD_TABLE_AUTOSIZE_ROWS:	return( LNG("[CMD] Fit Row Sizes") );

	case ID_CMD_DIAGRAM_PARAMETERS:		return( LNG("[CMD] Properties") );

	case ID_CMD_SCATTERPLOT_PARAMETERS:	return( LNG("[CMD] Properties") );
	case ID_CMD_SCATTERPLOT_UPDATE:		return( LNG("[CMD] Update Data") );

	case ID_CMD_HISTOGRAM_CUMULATIVE:	return( LNG("[CMD] Cumulative") );
	case ID_CMD_HISTOGRAM_AS_TABLE:		return( LNG("[CMD] Convert To Table") );
	}

	//-----------------------------------------------------
	return( LNG("[CMD] UNRECOGNISED COMMAND ID") );
}

//---------------------------------------------------------
const wxChar * CMD_Get_Help(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	case ID_CMD_FRAME_QUIT:				return( LNG("[CMD] Exit SAGA") );
	case ID_CMD_FRAME_ABOUT:			return( LNG("[CMD] About SAGA") );
	}

	return( CMD_Get_Name(Cmd_ID) );
}

//---------------------------------------------------------
int CMD_Get_ImageID(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	case ID_CMD_FRAME_WKSP_SHOW:		return( ID_IMG_TB_WKSP );
	case ID_CMD_FRAME_INFO_SHOW:		return( ID_IMG_TB_INFO );
	case ID_CMD_FRAME_ACTIVE_SHOW:		return( ID_IMG_TB_ACTIVE );
	case ID_CMD_FRAME_HELP:				return( ID_IMG_TB_HELP );
	case ID_CMD_WKSP_OPEN:				return( ID_IMG_TB_OPEN );
	case ID_CMD_MODULES_OPEN:			return( ID_IMG_TB_OPEN_MODULE );
	case ID_CMD_TABLES_OPEN:			return( ID_IMG_TB_OPEN_TABLE );
	case ID_CMD_SHAPES_OPEN:			return( ID_IMG_TB_OPEN_SHAPES );
	case ID_CMD_GRIDS_OPEN:				return( ID_IMG_TB_OPEN_GRID );
	case ID_CMD_TIN_OPEN:				return( ID_IMG_TB_OPEN_TIN );

	case ID_CMD_MAP_MODE_DISTANCE:		return( ID_IMG_TB_MAP_MODE_DISTANCE );
	case ID_CMD_MAP_MODE_SELECT:		return( ID_IMG_TB_MAP_MODE_SELECT );
	case ID_CMD_MAP_MODE_ZOOM:			return( ID_IMG_TB_MAP_MODE_ZOOM );
	case ID_CMD_MAP_MODE_PAN:			return( ID_IMG_TB_MAP_MODE_PAN );
	case ID_CMD_MAP_ZOOM_FULL:			return( ID_IMG_TB_MAP_ZOOM_FULL );
	case ID_CMD_MAP_ZOOM_LAST:			return( ID_IMG_TB_MAP_ZOOM_LAST );
	case ID_CMD_MAP_ZOOM_ACTIVE:		return( ID_IMG_TB_MAP_ZOOM_ACTIVE );
	case ID_CMD_MAP_ZOOM_SELECTION:		return( ID_IMG_TB_MAP_ZOOM_SELECTION );
	case ID_CMD_MAP_ZOOM_EXTENT:		return( ID_IMG_TB_MAP_ZOOM_SELECTION );
	case ID_CMD_MAP_3D_SHOW:			return( ID_IMG_TB_MAP_3D_SHOW );
	case ID_CMD_MAPS_3D_SHOW:			return( ID_IMG_TB_MAP_3D_SHOW );
	case ID_CMD_MAP_LAYOUT_SHOW:		return( ID_IMG_TB_MAP_LAYOUT_SHOW );
	case ID_CMD_MAPS_LAYOUT_SHOW:		return( ID_IMG_TB_MAP_LAYOUT_SHOW );
	case ID_CMD_MAPS_SAVE_IMAGE:
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
void CMD_ToolBar_Add(wxToolBarBase *pToolBar, const wxChar *Name)
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
