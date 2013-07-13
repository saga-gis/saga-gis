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
//                     RES_Images.h                      //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__RES_Images_H
#define _HEADER_INCLUDED__SAGA_GUI__RES_Images_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/bitmap.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum ID_IMAGES
{
	ID_IMG_DEFAULT,
	ID_IMG_NB_ACTIVE_ATTRIBUTES,
	ID_IMG_NB_ACTIVE_DESCRIPTION,
	ID_IMG_NB_ACTIVE_HISTORY,
	ID_IMG_NB_ACTIVE_LEGEND,
	ID_IMG_NB_ACTIVE_PARAMETERS,
	ID_IMG_NB_ACTIVE_HTMLEXTRAINFO,
	ID_IMG_NB_DATA_SOURCE_FILES,
	ID_IMG_NB_DATA_SOURCE_DATABASE,
	ID_IMG_NB_DATA_SOURCE_WEBSERVICE,
	ID_IMG_NB_INFO_ERROR,
	ID_IMG_NB_INFO_EXECUTION,
	ID_IMG_NB_INFO_MESSAGES,
	ID_IMG_NB_WKSP_DATA,
	ID_IMG_NB_WKSP_DATA_BUTTONS,
	ID_IMG_NB_WKSP_MAPS,
	ID_IMG_NB_WKSP_MAPS_BUTTONS,
	ID_IMG_NB_WKSP_MODULES,
	ID_IMG_NB_WKSP_THUMBNAILS,
	ID_IMG_NB_WKSP_TREEVIEW,
	ID_IMG_SAGA_ICON_16,
	ID_IMG_SAGA_ICON_32,
	ID_IMG_SAGA_SPLASH,
	ID_IMG_WND_DIAGRAM,
	ID_IMG_WND_HISTOGRAM,
	ID_IMG_WND_LAYOUT,
	ID_IMG_WND_MAP,
	ID_IMG_WND_MAP3D,
	ID_IMG_WND_SCATTERPLOT,
	ID_IMG_WND_TABLE,

	ID_IMG_CRS_EDIT_POINT_ADD,
	ID_IMG_CRS_EDIT_POINT_MOVE,
	ID_IMG_CRS_HAND,
	ID_IMG_CRS_HAND_GRAP,
	ID_IMG_CRS_INFO,
	ID_IMG_CRS_MAGNIFIER,
	ID_IMG_CRS_SELECT,

	ID_IMG_TB_ACTIVE,
	ID_IMG_TB_DATASOURCE,
	ID_IMG_TB_DELETE,
	ID_IMG_TB_DIAGRAM_PARAMETERS,
	ID_IMG_TB_EDIT_SHAPE_ADD,
	ID_IMG_TB_EDIT_SHAPE_ADD_PART,
	ID_IMG_TB_EDIT_SHAPE_DEL,
	ID_IMG_TB_EDIT_SHAPE_DEL_PART,
	ID_IMG_TB_EDIT_SHAPE_DEL_POINT,
	ID_IMG_TB_EDIT_SHAPE_SELECT,
	ID_IMG_TB_HELP,
	ID_IMG_TB_HISTOGRAM_AS_TABLE,
	ID_IMG_TB_HISTOGRAM_CUMULATIVE,
	ID_IMG_TB_INFO,
	ID_IMG_TB_LAYOUT_PAGE_SETUP,
	ID_IMG_TB_LAYOUT_PRINT,
	ID_IMG_TB_LAYOUT_PRINT_PREVIEW,
	ID_IMG_TB_LAYOUT_PRINT_SETUP,
	ID_IMG_TB_MAP3D_CENTRAL_LESS,
	ID_IMG_TB_MAP3D_CENTRAL_MORE,
	ID_IMG_TB_MAP3D_EXAGGERATE_LESS,
	ID_IMG_TB_MAP3D_EXAGGERATE_MORE,
	ID_IMG_TB_MAP3D_INTERPOLATED,
	ID_IMG_TB_MAP3D_PROPERTIES,
	ID_IMG_TB_MAP3D_ROTATE_X_LESS,
	ID_IMG_TB_MAP3D_ROTATE_X_MORE,
	ID_IMG_TB_MAP3D_ROTATE_Z_LESS,
	ID_IMG_TB_MAP3D_ROTATE_Z_MORE,
	ID_IMG_TB_MAP3D_SHIFT_X_LESS,
	ID_IMG_TB_MAP3D_SHIFT_X_MORE,
	ID_IMG_TB_MAP3D_SHIFT_Y_LESS,
	ID_IMG_TB_MAP3D_SHIFT_Y_MORE,
	ID_IMG_TB_MAP3D_SHIFT_Z_LESS,
	ID_IMG_TB_MAP3D_SHIFT_Z_MORE,
	ID_IMG_TB_MAP3D_STEREO,
	ID_IMG_TB_MAP_3D_SHOW,
	ID_IMG_TB_MAP_LAYOUT_SHOW,
	ID_IMG_TB_MAP_LEGEND_HORIZONTAL,
	ID_IMG_TB_MAP_LEGEND_VERTICAL,
	ID_IMG_TB_MAP_MODE_DISTANCE,
	ID_IMG_TB_MAP_MODE_PAN,
	ID_IMG_TB_MAP_MODE_SELECT,
	ID_IMG_TB_MAP_MODE_ZOOM,
	ID_IMG_TB_MAP_SCALEBAR,
	ID_IMG_TB_MAP_SYNCHRONIZE,
	ID_IMG_TB_MAP_ZOOM_ACTIVE,
	ID_IMG_TB_MAP_ZOOM_FULL,
	ID_IMG_TB_MAP_ZOOM_LAST,
	ID_IMG_TB_MAP_ZOOM_NEXT,
	ID_IMG_TB_MAP_ZOOM_SELECTION,
	ID_IMG_TB_OPEN,
	ID_IMG_TB_OPEN_GRID,
	ID_IMG_TB_OPEN_MODULE,
	ID_IMG_TB_OPEN_SHAPES,
	ID_IMG_TB_OPEN_TABLE,
	ID_IMG_TB_OPEN_TIN,
	ID_IMG_TB_OPEN_POINTCLOUD,
	ID_IMG_TB_SAVE,
	ID_IMG_TB_SCATTERPLOT_PARAMETERS,
	ID_IMG_TB_SHOW,
	ID_IMG_TB_TABLE_COL_ADD,
	ID_IMG_TB_TABLE_COL_DEL,
	ID_IMG_TB_TABLE_ROW_ADD,
	ID_IMG_TB_TABLE_ROW_DEL,
	ID_IMG_TB_TABLE_ROW_DEL_ALL,
	ID_IMG_TB_TABLE_ROW_INS,
	ID_IMG_TB_WKSP,

	ID_IMG_WKSP_DATA_MANAGER,
	ID_IMG_WKSP_GRID,
	ID_IMG_WKSP_GRID_MANAGER,
	ID_IMG_WKSP_GRID_SYSTEM,
	ID_IMG_WKSP_MAP,
	ID_IMG_WKSP_MAP_MANAGER,
	ID_IMG_WKSP_MODULE,
	ID_IMG_WKSP_MODULE_LIBRARY,
	ID_IMG_WKSP_MODULE_MANAGER,
	ID_IMG_WKSP_NOITEMS,
	ID_IMG_WKSP_ODBC_SOURCES,
	ID_IMG_WKSP_ODBC_SOURCE_ON,
	ID_IMG_WKSP_ODBC_SOURCE_OFF,
	ID_IMG_WKSP_ODBC_TABLE,
	ID_IMG_WKSP_SHAPES_LINE,
	ID_IMG_WKSP_SHAPES_MANAGER,
	ID_IMG_WKSP_SHAPES_POINT,
	ID_IMG_WKSP_SHAPES_POINTS,
	ID_IMG_WKSP_SHAPES_POLYGON,
	ID_IMG_WKSP_TABLE,
	ID_IMG_WKSP_TABLE_MANAGER,
	ID_IMG_WKSP_TIN,
	ID_IMG_WKSP_TIN_MANAGER,
	ID_IMG_WKSP_POINTCLOUD,
	ID_IMG_WKSP_POINTCLOUD_MANAGER
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define IMG_SIZE_NOTEBOOK			16
#define IMG_SIZE_TREECTRL			16

//---------------------------------------------------------
#define IMG_ADD_TO_NOTEBOOK(id)		GetImageList()->Add(IMG_Get_Bitmap(id, IMG_SIZE_NOTEBOOK));
#define IMG_ADD_TO_TREECTRL(id)		GetImageList()->Add(IMG_Get_Bitmap(id, IMG_SIZE_TREECTRL));

//---------------------------------------------------------
wxImage			IMG_Get_Image	(int ID_IMG);
wxImage			IMG_Get_Image	(int ID_IMG, int size);
wxBitmap		IMG_Get_Bitmap	(int ID_IMG);
wxBitmap		IMG_Get_Bitmap	(int ID_IMG, int size);
wxIcon			IMG_Get_Icon	(int ID_IMG);
wxCursor		IMG_Get_Cursor	(int ID_IMG);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__RES_Images_H
