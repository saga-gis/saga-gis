
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
	case ID_CMD_FRAME_QUIT:				return( _TL("Exit") );

	case ID_CMD_FRAME_HELP:				return( _TL("Help") );
	case ID_CMD_FRAME_ABOUT:			return( _TL("About SAGA") );
	case ID_CMD_FRAME_TIPS:				return( _TL("Tip of the Day...") );

	case ID_CMD_FRAME_CASCADE:			return( _TL("Cascade") );
	case ID_CMD_FRAME_TILE_HORZ:		return( _TL("Tile Horizontally") );
	case ID_CMD_FRAME_TILE_VERT:		return( _TL("Tile Vertically") );
	case ID_CMD_FRAME_ARRANGEICONS:		return( _TL("Arrange Icons") );
	case ID_CMD_FRAME_UNSPLIT:			return( _TL("Unsplit") );
	case ID_CMD_FRAME_SPLIT_LEFT:		return( _TL("Left") );
	case ID_CMD_FRAME_SPLIT_RIGHT:		return( _TL("Right") );
	case ID_CMD_FRAME_SPLIT_TOP:		return( _TL("Top") );
	case ID_CMD_FRAME_SPLIT_BOTTOM:		return( _TL("Bottom") );
	case ID_CMD_FRAME_SPLIT_ALL_HORZ:	return( _TL("Horizontally") );
	case ID_CMD_FRAME_SPLIT_ALL_VERT:	return( _TL("Vertically") );
	case ID_CMD_FRAME_NEXT:				return( _TL("Next") );
	case ID_CMD_FRAME_PREVIOUS:			return( _TL("Previous") );
	case ID_CMD_FRAME_CLOSE:			return( _TL("Close") );
	case ID_CMD_FRAME_CLOSE_ALL:		return( _TL("Close All") );
	case ID_CMD_FRAME_CLOSE_ALL_OTHERS:	return( _TL("Close All But This") );

	case ID_CMD_FRAME_INFO_SHOW:		return( _TL("Show Message Window") );
	case ID_CMD_FRAME_DATA_SOURCE_SHOW:	return( _TL("Show Data Source Window") );
	case ID_CMD_FRAME_ACTIVE_SHOW:		return( _TL("Show Object Properties Window") );
	case ID_CMD_FRAME_WKSP_SHOW:		return( _TL("Show Manager Window") );


	//-----------------------------------------------------
	// Workspace...

	case ID_CMD_WKSP_ITEM_CLOSE:		return( _TL("Close") );
	case ID_CMD_WKSP_ITEM_SHOW:			return( _TL("Show") );
	case ID_CMD_WKSP_ITEM_SETTINGS_LOAD:return( _TL("Load Settings") );
	case ID_CMD_WKSP_ITEM_SETTINGS_COPY:return( _TL("Copy Settings from other Layer") );
	case ID_CMD_WKSP_ITEM_SEARCH:		return( _TL("Search for...") );

	case ID_CMD_INFO_CLEAR:				return( _TL("Clear") );
	case ID_CMD_INFO_COPY:				return( _TL("Copy") );
	case ID_CMD_INFO_SELECTALL:			return( _TL("Select All") );

	case ID_CMD_DATASOURCE_REFRESH:		return( _TL("Refresh") );
	case ID_CMD_DATASOURCE_OPEN:		return( _TL("Open") );

	case ID_CMD_DB_REFRESH:				return( _TL("Refresh") );
	case ID_CMD_DB_SOURCE_CREATE:		return( _TL("Create Database") );
	case ID_CMD_DB_SOURCE_DROP:			return( _TL("Drop Database") );
	case ID_CMD_DB_SOURCE_OPEN:			return( _TL("Open Database") );
	case ID_CMD_DB_SOURCE_CLOSE:		return( _TL("Close Database") );
	case ID_CMD_DB_SOURCE_CLOSE_ALL:	return( _TL("Close All") );
	case ID_CMD_DB_SOURCE_DELETE:		return( _TL("Remove Database") );
	case ID_CMD_DB_SOURCE_SQL:			return( _TL("Execute SQL") );
	case ID_CMD_DB_TABLE_OPEN:			return( _TL("Load") );
	case ID_CMD_DB_TABLE_DELETE:		return( _TL("Delete") );
	case ID_CMD_DB_TABLE_RENAME:		return( _TL("Rename") );
	case ID_CMD_DB_TABLE_INFO:			return( _TL("Field Description") );
	case ID_CMD_DB_TABLE_FROM_QUERY:	return( _TL("Table from Query") );

	case ID_CMD_TOOL_OPEN:				return( _TL("Load Tool Library") );
	case ID_CMD_TOOL_RELOAD:			return( _TL("Reload Standard Tool Libraries") );
	case ID_CMD_TOOL_SEARCH:			return( _TL("Find and Run Tool") );
	case ID_CMD_TOOL_SAVE_DOCS:			return( _TL("Create Tool Description Files") );
	case ID_CMD_TOOL_SAVE_SCRIPT:		return( _TL("Save to Script File") );
	case ID_CMD_TOOL_SAVE_TO_CLIPBOARD:	return( _TL("Copy to Clipboard") );
	case ID_CMD_TOOL_CHAIN_RELOAD:		return( _TL("Reload") );
	case ID_CMD_TOOL_CHAIN_EDIT:		return( _TL("Edit") );

	case ID_CMD_DATA_PROJECT_OPEN:		return( _TL("Load Project") );
	case ID_CMD_DATA_PROJECT_OPEN_ADD:	return( _TL("Add Project") );
	case ID_CMD_DATA_PROJECT_BROWSE:	return( _TL("Search for Projects") );		
	case ID_CMD_DATA_PROJECT_NEW:		return( _TL("New Project") );
	case ID_CMD_DATA_PROJECT_SAVE:		return( _TL("Save Project") );
	case ID_CMD_DATA_PROJECT_SAVE_AS:	return( _TL("Save Project as...") );
	case ID_CMD_DATA_PROJECT_COPY:		return( _TL("Copy Project to...") );
	case ID_CMD_DATA_PROJECT_COPY_DB:	return( _TL("Copy Project to Database") );

	case ID_CMD_DATA_MANAGER_LIST:		return( _TL("Data Manager's Summary") );

	case ID_CMD_DATA_LEGEND_COPY:		return( _TL("Copy Legend to Clipboard") );
	case ID_CMD_DATA_LEGEND_SIZE_INC:	return( _TL("Increase Legend Size") );
	case ID_CMD_DATA_LEGEND_SIZE_DEC:	return( _TL("Decrease Legend Size") );
	case ID_CMD_DATA_LEGEND_BG_BLACK:	return( _TL("Black Background") );

	case ID_CMD_DATA_HISTORY_CLEAR:		return( _TL("Clear History") );
	case ID_CMD_DATA_HISTORY_TO_MODEL:	return( _TL("Save as Tool Chain") );
	case ID_CMD_DATA_HISTORY_OPTIONS_COLLAPSE:	return( _TL("Collpase Options") );
	case ID_CMD_DATA_HISTORY_OPTIONS_EXPAND:	return( _TL("Expand Options") );

	case ID_CMD_DATA_OPEN:				return( _TL("Open") );
	case ID_CMD_DATA_SAVE:				return( _TL("Save") );
	case ID_CMD_DATA_SAVEAS:			return( _TL("Save as...") );
	case ID_CMD_DATA_SAVETODB:			return( _TL("Save to Database...") );
	case ID_CMD_DATA_RELOAD:			return( _TL("Reload") );
	case ID_CMD_DATA_DEL_FILES:			return( _TL("Delete Associated Files") );
	case ID_CMD_DATA_PROJECTION:		return( _TL("Spatial Reference") );
	case ID_CMD_DATA_METADATA:			return( _TL("View Metadata") );
	case ID_CMD_DATA_FORCE_UPDATE:		return( _TL("Force Update") );

	case ID_CMD_GRID_OPEN:
	case ID_CMD_GRIDS_OPEN:
	case ID_CMD_SHAPES_OPEN:
	case ID_CMD_TABLE_OPEN:
	case ID_CMD_TIN_OPEN:
	case ID_CMD_POINTCLOUD_OPEN:		return( _TL("Load") );

	case ID_CMD_GRID_SHOW:
	case ID_CMD_GRIDS_SHOW:
	case ID_CMD_SHAPES_SHOW:
	case ID_CMD_TIN_SHOW:
	case ID_CMD_POINTCLOUD_SHOW:		return( _TL("Add to Map") );

	case ID_CMD_TABLE_SHOW:				return( _TL("Show") );

	case ID_CMD_GRID_SET_LUT:
	case ID_CMD_SHAPES_SET_LUT:			return( _TL("Classify") );

	case ID_CMD_DATA_CLASSIFY_IMPORT:	return( _TL("Import Classification") );

	case ID_CMD_GRID_HISTOGRAM:
	case ID_CMD_SHAPES_HISTOGRAM:		return( _TL("Histogram") );

	case ID_CMD_GRID_SCATTERPLOT:
	case ID_CMD_GRIDS_SCATTERPLOT:
	case ID_CMD_TABLE_SCATTERPLOT:		return( _TL("Scatterplot") );

	case ID_CMD_TABLE_DIAGRAM:			return( _TL("Diagram") );

	case ID_CMD_GRID_SAVEAS_IMAGE:		return( _TL("Save as Image...") );
	case ID_CMD_GRID_CLIPBOARD_IMAGE:	return( _TL("Copy to Clipboard") );
	case ID_CMD_GRID_EQUALINTERVALS:	return( _TL("Create Normalised Classification") );
	case ID_CMD_GRID_SEL_CLEAR:			return( _TL("Clear Selection") );
	case ID_CMD_GRID_SEL_DELETE:		return( _TL("Delete Selection") );

	case ID_CMD_GRIDS_SAVEAS_IMAGE:		return( _TL("Save as Image...") );
	case ID_CMD_GRIDS_FIT_MINMAX:		return( _TL("Histogram Stretch to Minimum/Maximum") );
	case ID_CMD_GRIDS_FIT_STDDEV:		return( _TL("Histogram Stretch to Standard Deviation") );
	case ID_CMD_GRIDS_FIT_PCTL  :		return( _TL("Histogram Stretch to Percentile") );
	case ID_CMD_GRIDS_FIT_DIALOG:		return( _TL("Histogram Stretch Dialog") );

	case ID_CMD_SHAPES_SAVE_ATTRIBUTES:	return( _TL("Save Attributes as...") );
	case ID_CMD_SHAPES_EDIT_SHAPE:		return( _TL("Edit Selection") );
	case ID_CMD_SHAPES_EDIT_SEL_CLEAR:	return( _TL("Clear Selection") );
	case ID_CMD_SHAPES_EDIT_SEL_INVERT:	return( _TL("Invert Selection") );
	case ID_CMD_SHAPES_EDIT_ADD_SHAPE:	return( _TL("Add Shape") );
	case ID_CMD_SHAPES_EDIT_ADD_PART:	return( _TL("Add Part") );
	case ID_CMD_SHAPES_EDIT_DEL_SHAPE:	return( _TL("Delete Selection") );
	case ID_CMD_SHAPES_EDIT_DEL_PART:	return( _TL("Delete Selected Part") );
	case ID_CMD_SHAPES_EDIT_DEL_POINT:	return( _TL("Delete Selected Point") );
	case ID_CMD_SHAPES_EDIT_SPLIT:		return( _TL("Split with Line") );
	case ID_CMD_SHAPES_EDIT_MERGE:		return( _TL("Merge Selection") );
	case ID_CMD_SHAPES_EDIT_MOVE:		return( _TL("Move") );

	case ID_CMD_POINTCLOUD_RANGE_MINMAX:		return( _TL("Set Range to Minimum/Maximum") );
	case ID_CMD_POINTCLOUD_RANGE_STDDEV150:		return( _TL("Set Range to Standard Deviation (1.5)") );
	case ID_CMD_POINTCLOUD_RANGE_STDDEV200:		return( _TL("Set Range to Standard Deviation (2.0)") );

	case ID_CMD_MAPS_SHOW:						return( _TL("Show Map") );
	case ID_CMD_MAPS_3D_SHOW:					return( _TL("Show 3D-View") );
	case ID_CMD_MAPS_LAYOUT_SHOW:				return( _TL("Show Print Layout") );
	case ID_CMD_MAPS_SAVE_IMAGE:				return( _TL("Save as Image...") );
	case ID_CMD_MAPS_SAVE_IMAGE_ON_CHANGE:		return( _TL("Save as Image when changed") );
	case ID_CMD_MAPS_SAVE_IMAGE_TO_MEMORY:		return( _TL("Save as Image to Workspace") );
	case ID_CMD_MAPS_SAVE_IMAGE_TO_KMZ:			return( _TL("Export Map to Google Earth") );
	case ID_CMD_MAPS_SAVE_TO_CLIPBOARD:			return( _TL("Copy to Clipboard") );
	case ID_CMD_MAPS_SAVE_TO_CLIPBOARD_LEGEND:	return( _TL("Copy Legend to Clipboard") );
	case ID_CMD_MAPS_SYNCHRONIZE:				return( _TL("Synchronise Map Extents") );
	case ID_CMD_MAPS_SCALEBAR:					return( _TL("Scale Bar") );
	case ID_CMD_MAPS_PROJECTION:				return( _TL("Spatial Reference") );
	case ID_CMD_MAPS_GRATICULE_ADD:				return( _TL("Add Graticule") );
	case ID_CMD_MAPS_BASEMAP_ADD:				return( _TL("Add Base Map") );
	case ID_CMD_MAPS_LAYER_SHOW:				return( _TL("Show Layer") );
	case ID_CMD_MAPS_MOVE_TOP:					return( _TL("Move To Top") );
	case ID_CMD_MAPS_MOVE_BOTTOM:				return( _TL("Move To Bottom") );
	case ID_CMD_MAPS_MOVE_UP:					return( _TL("Move Up") );
	case ID_CMD_MAPS_MOVE_DOWN:					return( _TL("Move Down") );
	case ID_CMD_MAPS_GRID_FITCOLORS:			return( _TL("Adjust Histogram Stretch to Map Extent") );
	case ID_CMD_MAPS_PROJECT:					return( _TL("On-The-Fly Projection") );
	case ID_CMD_MAPS_PROJECT_AREA:				return( _TL("Restricted Area Projection") );

	//-----------------------------------------------------
	// Views...

	case ID_CMD_MAP_CLOSE:						return( _TL("Close") );
	case ID_CMD_MAP_TOOLBAR:					return( _TL("Show Toolbar") );
	case ID_CMD_MAP_3D_SHOW:					return( CMD_Get_Name(ID_CMD_MAPS_3D_SHOW) );
	case ID_CMD_MAP_LAYOUT_SHOW:				return( CMD_Get_Name(ID_CMD_MAPS_LAYOUT_SHOW) );
	case ID_CMD_MAP_SCALEBAR:					return( CMD_Get_Name(ID_CMD_MAPS_SCALEBAR) );
	case ID_CMD_MAP_GRATICULE_ADD:				return( CMD_Get_Name(ID_CMD_MAPS_GRATICULE_ADD) );
	case ID_CMD_MAP_BASEMAP_ADD:				return( CMD_Get_Name(ID_CMD_MAPS_BASEMAP_ADD) );
	case ID_CMD_MAP_SAVE_IMAGE:					return( CMD_Get_Name(ID_CMD_MAPS_SAVE_IMAGE) );
	case ID_CMD_MAP_SAVE_IMAGE_ON_CHANGE:		return( CMD_Get_Name(ID_CMD_MAPS_SAVE_IMAGE_ON_CHANGE) );
	case ID_CMD_MAP_SYNCHRONIZE:				return( CMD_Get_Name(ID_CMD_MAPS_SYNCHRONIZE) );
	case ID_CMD_MAP_NORTH_ARROW:				return( _TL("North Arrow") );
	case ID_CMD_MAP_LEGEND_VERTICAL:			return( _TL("Show Vertical Legend") );
	case ID_CMD_MAP_LEGEND_HORIZONTAL:			return( _TL("Show Horizontal Legend") );
	case ID_CMD_MAP_MODE_ZOOM:					return( _TL("Zoom") );
	case ID_CMD_MAP_MODE_PAN:					return( _TL("Pan") );
	case ID_CMD_MAP_MODE_SELECT:				return( _TL("Action") );
	case ID_CMD_MAP_MODE_DISTANCE:				return( _TL("Measure Distance") );
	case ID_CMD_MAP_ZOOM_FULL:					return( _TL("Zoom To Full Extent") );
	case ID_CMD_MAP_ZOOM_BACK:					return( _TL("Zoom To Previous Extent") );
	case ID_CMD_MAP_ZOOM_FORWARD:				return( _TL("Zoom To Next Extent") );
	case ID_CMD_MAP_ZOOM_ACTIVE:				return( _TL("Zoom To Active Layer") );
	case ID_CMD_MAP_PAN_ACTIVE:					return( _TL("Pan To Active Layer") );
	case ID_CMD_MAP_ZOOM_SELECTION:				return( _TL("Zoom To Selection") );
	case ID_CMD_MAP_ZOOM_EXTENT:				return( _TL("Zoom To Extent") );
	case ID_CMD_MAP_CROSSHAIR:					return( _TL("Cross Hair") );

	case ID_CMD_MAP3D_PARAMETERS:		return( _TL("Properties") );
	case ID_CMD_MAP3D_ROTATE_X_LESS:	return( _TL("Up") );
	case ID_CMD_MAP3D_ROTATE_X_MORE:	return( _TL("Down") );
	case ID_CMD_MAP3D_ROTATE_Y_LESS:	return( _TL("Left [F3]") );
	case ID_CMD_MAP3D_ROTATE_Y_MORE:	return( _TL("Right [F4]") );
	case ID_CMD_MAP3D_ROTATE_Z_LESS:	return( _TL("Left") );
	case ID_CMD_MAP3D_ROTATE_Z_MORE:	return( _TL("Right") );
	case ID_CMD_MAP3D_SHIFT_X_LESS:		return( _TL("Left") );
	case ID_CMD_MAP3D_SHIFT_X_MORE:		return( _TL("Right") );
	case ID_CMD_MAP3D_SHIFT_Y_LESS:		return( _TL("Down") );
	case ID_CMD_MAP3D_SHIFT_Y_MORE:		return( _TL("Up") );
	case ID_CMD_MAP3D_SHIFT_Z_LESS:		return( _TL("Forward") );
	case ID_CMD_MAP3D_SHIFT_Z_MORE:		return( _TL("Backward") );
	case ID_CMD_MAP3D_EXAGGERATE_LESS:	return( _TL("Decrease Exaggeration [F1]") );
	case ID_CMD_MAP3D_EXAGGERATE_MORE:	return( _TL("Increase Exaggeration [F2]") );
	case ID_CMD_MAP3D_CENTRAL_LESS:		return( _TL("Decrease Perspectivic Distance [F9]") );
	case ID_CMD_MAP3D_CENTRAL_MORE:		return( _TL("Increase Perspectivic Distance [F10]") );
	case ID_CMD_MAP3D_CENTRAL:			return( _TL("Central Projection") );
	case ID_CMD_MAP3D_STEREO_LESS:		return( _TL("Decrease Eye Distance") );
	case ID_CMD_MAP3D_STEREO_MORE:		return( _TL("Increase Eye Distance") );
	case ID_CMD_MAP3D_STEREO:			return( _TL("Anaglyph [S]") );
	case ID_CMD_MAP3D_INTERPOLATED:		return( _TL("Interpolated Colors") );
	case ID_CMD_MAP3D_SAVE:				return( _TL("Save as Image...") );
	case ID_CMD_MAP3D_SEQ_POS_ADD:		return( _TL("Add Position [Ctrl + A]") );
	case ID_CMD_MAP3D_SEQ_POS_DEL:		return( _TL("Delete Last Position [Ctrl + D]") );
	case ID_CMD_MAP3D_SEQ_POS_DEL_ALL:	return( _TL("Delete All Positions") );
	case ID_CMD_MAP3D_SEQ_POS_EDIT:		return( _TL("Edit Positions") );
	case ID_CMD_MAP3D_SEQ_PLAY:			return( _TL("Play Once [Ctrl + P]") );
	case ID_CMD_MAP3D_SEQ_PLAY_LOOP:	return( _TL("Play Loop [Ctrl + L]") );
	case ID_CMD_MAP3D_SEQ_SAVE:			return( _TL("Play And Save as Images...") );

	case ID_CMD_LAYOUT_LOAD:			return( _TL("Load") );
	case ID_CMD_LAYOUT_SAVE:			return( _TL("Save") );
	case ID_CMD_LAYOUT_PAGE_SETUP:		return( _TL("Page Setup") );
	case ID_CMD_LAYOUT_PRINT_SETUP:		return( _TL("Print Setup") );
	case ID_CMD_LAYOUT_PRINT_PREVIEW:	return( _TL("Print Preview") );
	case ID_CMD_LAYOUT_PRINT:			return( _TL("Print") );
	case ID_CMD_LAYOUT_EXPORT:			return( _TL("Export") );
	case ID_CMD_LAYOUT_TO_CLIPBOARD:	return( _TL("Copy to Clipboard") );
	case ID_CMD_LAYOUT_PROPERTIES:		return( _TL("Properties") );
	case ID_CMD_LAYOUT_ZOOM_IN:			return( _TL("Zoom In") );
	case ID_CMD_LAYOUT_ZOOM_OUT:		return( _TL("Zoom Out") );
	case ID_CMD_LAYOUT_ZOOM_FULL:		return( _TL("Zoom Whole Page") );
	case ID_CMD_LAYOUT_ZOOM_ORIGINAL:	return( _TL("Zoom to 100%") );
	case ID_CMD_LAYOUT_ITEM_MAP:		return( _TL("Map") );
	case ID_CMD_LAYOUT_ITEM_LEGEND:		return( _TL("Legend") );
	case ID_CMD_LAYOUT_ITEM_SCALEBAR:	return( _TL("Scalebar") );
	case ID_CMD_LAYOUT_ITEM_SCALE:		return( _TL("Scale") );
	case ID_CMD_LAYOUT_ITEM_LABEL:		return( _TL("Label") );
	case ID_CMD_LAYOUT_ITEM_TEXT:		return( _TL("Text") );
	case ID_CMD_LAYOUT_ITEM_IMAGE:		return( _TL("Image") );
	case ID_CMD_LAYOUT_ITEM_PASTE:		return( _TL("Paste from Clipboard") );
	case ID_CMD_LAYOUT_ITEM_PROPERTIES:	return( _TL("Properties") );
	case ID_CMD_LAYOUT_ITEM_HIDE:		return( _TL("Hide") );
	case ID_CMD_LAYOUT_ITEM_DELETE:		return( _TL("Delete") );
	case ID_CMD_LAYOUT_ITEM_MOVE_TOP:	return( _TL("Bring to Front") );
	case ID_CMD_LAYOUT_ITEM_MOVE_BOTTOM:return( _TL("Send to Back") );
	case ID_CMD_LAYOUT_ITEM_MOVE_UP:	return( _TL("Bring Forward") );
	case ID_CMD_LAYOUT_ITEM_MOVE_DOWN:	return( _TL("Send Backward") );
	case ID_CMD_LAYOUT_IMAGE_SAVE:		return( _TL("Save") );
	case ID_CMD_LAYOUT_IMAGE_RESTORE:	return( _TL("Restore") );

	case ID_CMD_TABLE_FIELD_ADD:		return( _TL("Add Field") );
	case ID_CMD_TABLE_FIELD_MOVE:		return( _TL("Move Field") );
	case ID_CMD_TABLE_FIELD_DEL:		return( _TL("Delete Fields") );
	case ID_CMD_TABLE_FIELD_HIDE:		return( _TL("Hide Fields") );
	case ID_CMD_TABLE_FIELD_SORT:		return( _TL("Sort Fields") );
	case ID_CMD_TABLE_FIELD_RENAME:		return( _TL("Rename Fields") );
	case ID_CMD_TABLE_FIELD_TYPE:		return( _TL("Change Field Type") );
	case ID_CMD_TABLE_FIELD_CALC:		return( _TL("Field Calculator") );
	case ID_CMD_TABLE_FIELD_OPEN_APP:	return( _TL("Open with external application") );
	case ID_CMD_TABLE_FIELD_OPEN_DATA:	return( _TL("Open as data set") );
	case ID_CMD_TABLE_RECORD_ADD:		return( _TL("Add Record") );
	case ID_CMD_TABLE_RECORD_INS:		return( _TL("Insert Record") );
	case ID_CMD_TABLE_RECORD_DEL:		return( _TL("Delete Selection") );
	case ID_CMD_TABLE_RECORD_DEL_ALL:	return( _TL("Delete All Records") );
	case ID_CMD_TABLE_SELECTION_CLEAR:	return( _TL("Clear Selection") );
	case ID_CMD_TABLE_SELECTION_ONLY:	return( _TL("Show Selection") );
	case ID_CMD_TABLE_AUTOSIZE_COLS:	return( _TL("Fit Column Sizes") );
	case ID_CMD_TABLE_AUTOSIZE_ROWS:	return( _TL("Fit Row Sizes") );
	case ID_CMD_TABLE_TO_CLIPBOARD:		return( _TL("Copy to Clipboard") );

	case ID_CMD_DIAGRAM_PARAMETERS:		return( _TL("Properties") );
	case ID_CMD_DIAGRAM_SIZE_FIT:		return( _TL("Fit Size") );
	case ID_CMD_DIAGRAM_SIZE_INC:		return( _TL("Zoom In [PgDn]") );
	case ID_CMD_DIAGRAM_SIZE_DEC:		return( _TL("Zoom Out [PgUp]") );
	case ID_CMD_DIAGRAM_LEGEND:			return( _TL("Legend") );
	case ID_CMD_DIAGRAM_TO_CLIPBOARD:	return( _TL("Copy to Clipboard") );

	case ID_CMD_SCATTERPLOT_PARAMETERS:		return( _TL("Properties") );
	case ID_CMD_SCATTERPLOT_UPDATE:			return( _TL("Update Data") );
	case ID_CMD_SCATTERPLOT_AS_TABLE:		return( _TL("Save to Table") );
	case ID_CMD_SCATTERPLOT_TO_CLIPBOARD:	return( _TL("Copy to Clipboard") );

	case ID_CMD_HISTOGRAM_PARAMETERS:		return( _TL("Properties") );
	case ID_CMD_HISTOGRAM_GAUSSIAN:			return( _TL("Normal Distribution") );
	case ID_CMD_HISTOGRAM_CUMULATIVE:		return( _TL("Cumulative") );
	case ID_CMD_HISTOGRAM_AS_TABLE:			return( _TL("Save to Table") );
	case ID_CMD_HISTOGRAM_TO_CLIPBOARD:		return( _TL("Copy to Clipboard") );
	case ID_CMD_HISTOGRAM_SET_MINMAX:		return( _TL("Adjust to Full Range") );
	}

	//-----------------------------------------------------
	return( wxString::Format("%s [%d]", _TL("unrecognized command identifier")) );
}

//---------------------------------------------------------
wxString CMD_Get_Help(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	case ID_CMD_FRAME_QUIT:				return( _TL("Exit SAGA") );
	case ID_CMD_FRAME_ABOUT:			return( _TL("About SAGA") );
	}

	return( CMD_Get_Name(Cmd_ID) );
}

//---------------------------------------------------------
int CMD_Get_ImageID(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	case ID_CMD_FRAME_WKSP_SHOW:		return( ID_IMG_TB_WKSP );
	case ID_CMD_FRAME_DATA_SOURCE_SHOW:	return( ID_IMG_TB_DATASOURCE );
	case ID_CMD_FRAME_ACTIVE_SHOW:		return( ID_IMG_TB_ACTIVE );
	case ID_CMD_FRAME_INFO_SHOW:		return( ID_IMG_TB_INFO );
	case ID_CMD_FRAME_HELP:				return( ID_IMG_TB_HELP );
	case ID_CMD_DATA_OPEN:				return( ID_IMG_TB_OPEN );
	case ID_CMD_DATA_PROJECT_SAVE:		return( ID_IMG_TB_SAVE );
	case ID_CMD_DATA_PROJECT_NEW:		return( ID_IMG_TB_NEW_PROJECT );
	case ID_CMD_TOOL_OPEN:				return( ID_IMG_TB_OPEN_TOOL );
	case ID_CMD_TOOL_SEARCH:			return( ID_IMG_NB_WKSP_TOOLS );
	case ID_CMD_TABLE_OPEN:				return( ID_IMG_TB_OPEN_TABLE );
	case ID_CMD_SHAPES_OPEN:			return( ID_IMG_TB_OPEN_SHAPES );
	case ID_CMD_GRID_OPEN:				return( ID_IMG_TB_OPEN_GRID );
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
	case ID_CMD_MAP_PAN_ACTIVE:			return( ID_IMG_TB_MAP_PAN_ACTIVE );
	case ID_CMD_MAP_ZOOM_SELECTION:		return( ID_IMG_TB_MAP_ZOOM_SELECTION );
	case ID_CMD_MAP_ZOOM_EXTENT:		break;
	case ID_CMD_MAP_CROSSHAIR:			return( ID_IMG_TB_MAP_CROSSHAIR );
	case ID_CMD_MAP_3D_SHOW:			return( ID_IMG_TB_MAP_3D_SHOW );
	case ID_CMD_MAPS_3D_SHOW:			return( ID_IMG_TB_MAP_3D_SHOW );
	case ID_CMD_MAP_LAYOUT_SHOW:		return( ID_IMG_TB_MAP_LAYOUT_SHOW );
	case ID_CMD_MAPS_LAYOUT_SHOW:		return( ID_IMG_TB_MAP_LAYOUT_SHOW );
	case ID_CMD_MAPS_SAVE_TO_CLIPBOARD:			return( ID_IMG_TB_CLIPBOARD );
	case ID_CMD_MAPS_SAVE_TO_CLIPBOARD_LEGEND:	break;
	case ID_CMD_MAPS_SAVE_IMAGE:				break;
	case ID_CMD_MAP_NORTH_ARROW:		return( ID_IMG_TB_MAP_NORTH_ARROW );
	case ID_CMD_MAP_SYNCHRONIZE:		return( ID_IMG_TB_MAP_SYNCHRONIZE );
	case ID_CMD_MAP_SCALEBAR:			return( ID_IMG_TB_MAP_SCALEBAR );
	case ID_CMD_MAP_GRATICULE_ADD:				break;
	case ID_CMD_MAP_BASEMAP_ADD:				break;
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

	case ID_CMD_LAYOUT_PAGE_SETUP:		return( ID_IMG_TB_LAYOUT_PAGE_SETUP );
	case ID_CMD_LAYOUT_PRINT_SETUP:		return( ID_IMG_TB_LAYOUT_PRINT_SETUP );
	case ID_CMD_LAYOUT_PRINT_PREVIEW:	return( ID_IMG_TB_LAYOUT_PRINT_PREVIEW );
	case ID_CMD_LAYOUT_PRINT:			return( ID_IMG_TB_LAYOUT_PRINT );
	case ID_CMD_LAYOUT_EXPORT:			return( ID_IMG_TB_SAVE );
	case ID_CMD_LAYOUT_TO_CLIPBOARD:	return( ID_IMG_TB_CLIPBOARD );
	case ID_CMD_LAYOUT_ZOOM_IN:			return( ID_IMG_TB_ZOOM_IN );
	case ID_CMD_LAYOUT_ZOOM_OUT:		return( ID_IMG_TB_ZOOM_OUT );
	case ID_CMD_LAYOUT_ZOOM_FULL:		return( ID_IMG_TB_ZOOM_FULL );
	case ID_CMD_LAYOUT_ZOOM_ORIGINAL:	return( ID_IMG_TB_ZOOM_ORIGINAL );

	case ID_CMD_TABLE_FIELD_ADD:		return( ID_IMG_TB_TABLE_COL_ADD );
	case ID_CMD_TABLE_FIELD_DEL:		return( ID_IMG_TB_TABLE_COL_DEL );
	case ID_CMD_TABLE_RECORD_ADD:		return( ID_IMG_TB_TABLE_ROW_ADD );
	case ID_CMD_TABLE_RECORD_INS:		return( ID_IMG_TB_TABLE_ROW_INS );
	case ID_CMD_TABLE_RECORD_DEL:		return( ID_IMG_TB_TABLE_ROW_DEL );
	case ID_CMD_TABLE_RECORD_DEL_ALL:	return( ID_IMG_TB_TABLE_ROW_DEL_ALL );
	case ID_CMD_TABLE_SELECTION_ONLY:	return( ID_IMG_TB_TABLE_SELECTION_ONLY );

	case ID_CMD_HISTOGRAM_PARAMETERS    :	return( ID_IMG_WND_HISTOGRAM );
	case ID_CMD_HISTOGRAM_CUMULATIVE    :	return( ID_IMG_TB_HISTOGRAM_CUMULATIVE );
	case ID_CMD_HISTOGRAM_GAUSSIAN      :	return( ID_IMG_TB_HISTOGRAM_GAUSSIAN );
	case ID_CMD_HISTOGRAM_AS_TABLE      :	return( ID_IMG_TB_HISTOGRAM_AS_TABLE );
	case ID_CMD_HISTOGRAM_TO_CLIPBOARD  :	return( ID_IMG_TB_CLIPBOARD );

	case ID_CMD_SCATTERPLOT_PARAMETERS  :	return( ID_IMG_TB_SCATTERPLOT_PARAMETERS );
	case ID_CMD_SCATTERPLOT_UPDATE      :	return( ID_IMG_TB_UPDATE );
	case ID_CMD_SCATTERPLOT_AS_TABLE    :	return( ID_IMG_TB_HISTOGRAM_AS_TABLE );
	case ID_CMD_SCATTERPLOT_TO_CLIPBOARD:	return( ID_IMG_TB_CLIPBOARD );

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
