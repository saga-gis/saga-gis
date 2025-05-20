
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
	ID_IMG_NB_ACTIVE_PARAMETERS,
	ID_IMG_NB_ACTIVE_DESCRIPTION,
	ID_IMG_NB_ACTIVE_HISTORY,
	ID_IMG_NB_ACTIVE_LEGEND,
	ID_IMG_NB_ACTIVE_ATTRIBUTES,
	ID_IMG_NB_ACTIVE_INFO,
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
	ID_IMG_NB_WKSP_TOOLS,
	ID_IMG_NB_WKSP_THUMBNAILS,
	ID_IMG_NB_WKSP_TREEVIEW,
	ID_IMG_SAGA_ICON,
	ID_IMG_SAGA_ICON_16,
	ID_IMG_SAGA_ICON_32,
	ID_IMG_SAGA_ICON_64,
	ID_IMG_SAGA_ICON_128,
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
	ID_IMG_TB_DATA_MAP_ADD,
	ID_IMG_TB_DATA_DELETE,
	ID_IMG_TB_DATA_SAVEAS,
	ID_IMG_TB_DATA_RELOAD,
	ID_IMG_TB_DATA_REFRESH,
	ID_IMG_TB_DATA_CRS,
	ID_IMG_TB_DATA_CLASSIFY,
	ID_IMG_TB_DATA_HISTOGRAM,
	ID_IMG_TB_DATA_SCATTERPLOT,
	ID_IMG_TB_DATA_DIAGRAM,
	ID_IMG_TB_DATA_TABLE_SHOW,
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
	ID_IMG_TB_HISTOGRAM_CUMULATIVE,
	ID_IMG_TB_HISTOGRAM_GAUSSIAN,
    ID_IMG_TB_HISTOGRAM_PARAMETERS,
	ID_IMG_TB_HISTOGRAM_AS_TABLE,
	ID_IMG_TB_INFO,
	ID_IMG_TB_UPDATE,
	ID_IMG_TB_CLIPBOARD,
	ID_IMG_TB_LAYOUT_PAGE_SETUP,
	ID_IMG_TB_LAYOUT_PRINT,
	ID_IMG_TB_LAYOUT_PRINT_PREVIEW,
	ID_IMG_TB_LAYOUT_PRINT_SETUP,
	ID_IMG_TB_LAYOUT_EXPORT,
	ID_IMG_TB_ZOOM_IN,
	ID_IMG_TB_ZOOM_OUT,
	ID_IMG_TB_ZOOM_FULL,
	ID_IMG_TB_ZOOM_ORIGINAL,
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
	ID_IMG_TB_MAP_NORTH_ARROW,
	ID_IMG_TB_MAP_SYNCHRONIZE,
	ID_IMG_TB_MAP_ZOOM_ACTIVE,
	ID_IMG_TB_MAP_PAN_ACTIVE,
	ID_IMG_TB_MAP_ZOOM_FULL,
	ID_IMG_TB_MAP_ZOOM_LAST,
	ID_IMG_TB_MAP_ZOOM_NEXT,
	ID_IMG_TB_MAP_ZOOM_SELECTION,
	ID_IMG_TB_MAP_CROSSHAIR,
	ID_IMG_TB_NEW_PROJECT,
	ID_IMG_TB_OPEN,
	ID_IMG_TB_OPEN_TOOL,
	ID_IMG_TB_SAVE,
	ID_IMG_TB_SCATTERPLOT_PARAMETERS,
	ID_IMG_TB_SHOW,
	ID_IMG_TB_TABLE_COL_ADD,
	ID_IMG_TB_TABLE_COL_DEL,
	ID_IMG_TB_TABLE_ROW_ADD,
	ID_IMG_TB_TABLE_ROW_DEL,
	ID_IMG_TB_TABLE_ROW_DEL_ALL,
	ID_IMG_TB_TABLE_ROW_INS,
	ID_IMG_TB_TABLE_SELECTION_ONLY,
	ID_IMG_TB_WKSP,

	ID_IMG_WKSP_DATA_MANAGER,
	ID_IMG_WKSP_GRID_MANAGER,
	ID_IMG_WKSP_GRID_SYSTEM,
	ID_IMG_WKSP_GRID,
	ID_IMG_WKSP_GRIDS,
	ID_IMG_WKSP_MAP,
	ID_IMG_WKSP_MAP_MANAGER,
	ID_IMG_WKSP_MAP_GRATICULE,
	ID_IMG_WKSP_MAP_BASEMAP,
	ID_IMG_WKSP_TOOL,
	ID_IMG_WKSP_TOOL_CHAIN,
	ID_IMG_WKSP_TOOL_LIBRARY,
	ID_IMG_WKSP_TOOL_GROUP,
	ID_IMG_WKSP_TOOL_MANAGER,
	ID_IMG_WKSP_NOITEMS,
	ID_IMG_WKSP_DB_SOURCES,
	ID_IMG_WKSP_DB_SOURCE_ON,
	ID_IMG_WKSP_DB_SOURCE_OFF,
	ID_IMG_WKSP_DB_TABLE,
	ID_IMG_WKSP_SHAPES_LINE,
	ID_IMG_WKSP_SHAPES_MANAGER,
	ID_IMG_WKSP_SHAPES_POINT,
	ID_IMG_WKSP_SHAPES_POINTS,
	ID_IMG_WKSP_SHAPES_POLYGON,
	ID_IMG_WKSP_SHAPES_GEOMETRY,
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
#ifdef wxHAS_SVG
	#define WITH_WXBMPBUNDLE // available since 3.1.6
#endif

//---------------------------------------------------------
#define IMG_SIZE_NOTEBOOK	16
#define IMG_SIZE_TREECTRL	16

//---------------------------------------------------------
#define IMG_ADD_TO_NOTEBOOK(id)		GetImageList()->Add(IMG_Get_Bitmap(id, wxSize(IMG_SIZE_NOTEBOOK, IMG_SIZE_NOTEBOOK)));
#define IMG_ADD_TO_TREECTRL(id)		GetImageList()->Add(IMG_Get_Bitmap(id, wxSize(IMG_SIZE_TREECTRL, IMG_SIZE_TREECTRL)));

//---------------------------------------------------------
#ifndef WITH_WXBMPBUNDLE
#define			IMG_Get_Bitmaps		IMG_Get_Bitmap
#else
#include <wx/bmpbndl.h>
wxBitmapBundle	IMG_Get_Bitmaps		(int ID_IMG);
wxBitmapBundle	IMG_Get_Bitmaps		(int ID_IMG, const wxSize &Size);

wxIconBundle	IMG_Get_Icons		(int ID_IMG);
#endif

wxBitmap		IMG_Get_Bitmap		(int ID_IMG);
wxBitmap		IMG_Get_Bitmap		(int ID_IMG, const wxSize &Size);

wxIcon			IMG_Get_Icon		(int ID_IMG);

wxCursor		IMG_Get_Cursor		(int ID_IMG);

wxBitmap		IMG_Get_Splash		(double Scale = 0.75);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__RES_Images_H
