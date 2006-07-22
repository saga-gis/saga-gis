
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_API                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   compatibility.h                     //
//                                                       //
//          Copyright (C) 2006 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'.                              //
//                                                       //
// This library is free software; you can redistribute   //
// it and/or modify it under the terms of the GNU Lesser //
// General Public License as published by the Free       //
// Software Foundation, version 2.1 of the License.      //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, write to the Free Software Foundation, Inc.,     //
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
#ifndef HEADER_INCLUDED__SAGA_API__compatibility_H
#define HEADER_INCLUDED__SAGA_API__compatibility_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
//
// This header provides some compatibility for SAGA modules
// developed on older API versions. 100% compatibility is
// not warranted, particularly not for 1.x version, but it
// may work for a whole part of it. For usage just include:
//
//     #include <saga_api/compatibility.h>
//
// instead of the standard header:
//
//     #include <saga_api/saga_api.h>
//
// Anyway, it is recommended to change your source codes to
// the current SAGA API conventions. I hope this file helps
// you to bridge the time until you get this done. O.C.
//
//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//				SAGA 1.2 Compatibility					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#if !defined(_TYPEDEF_BYTE)
	#define _TYPEDEF_BYTE
#endif

#if !defined(_TYPEDEF_WORD)
	#define _TYPEDEF_WORD
#endif

//---------------------------------------------------------
#include <saga_api/saga_api.h>

#include <memory.h>

//---------------------------------------------------------
typedef struct
{
	int		xMin, yMin, xMax, yMax;
}
TGEO_iRect;

//---------------------------------------------------------
#define Set_Callback(a)						//

#define PI2GRAD								M_RAD_TO_DEG
#define GRAD2PI								M_DEG_TO_RAD
#define M_PI_2								M_PI_090
#define M_PI_4								M_PI_045
#define ALMOST_ZERO							0.0001

#define RGB									COLOR_GET_RGB

#define SYS_Malloc							SG_Malloc
#define SYS_Calloc							SG_Calloc
#define SYS_Realloc							SG_Realloc
#define SYS_Free							SG_Free

#define Get_SafeNew_Grid					SG_Create_Grid
#define Get_DX								Get_Cellsize
#define Get_DY								Get_Cellsize
#define Get_Direction_Lowest(g, x, y)		(g)->Get_Gradient_NeighborDir(x, y)
#define _Get_Gradient(g, x, y, a, b)		(g)->Get_Gradient(x, y, a, b)
#define is_InGrid_Coord						is_InGrid_byPos
#define Get_Value_AtPosition				Get_Value

#define Add_Select							Add_Choice

#define Get_Default_Parameters				Get_Parameters

#define Progress_Set_Text					Process_Set_Text
#define Progress_Check						Set_Progress
#define Progress_Check_NXY					Set_Progress_NCells
#define Progress_Check_Blink()				Process_Get_Okay(true)

#define Get_NXY								Get_NCells
#define Get_xWorld							Get_xPosition_Last
#define Get_yWorld							Get_yPosition_Last
#define Set_Lock							Lock_Set
#define Get_Lock							Lock_Get
#define Set_Dimension(g)					Get_System()->Assign(g->Get_System())

#define On_Set_Position						Execute_Position

#define MODULE_INTERFACE_INFO_Name			MLB_INFO_Name
#define MODULE_INTERFACE_INFO_Author		MLB_INFO_Author
#define MODULE_INTERFACE_INFO_Description	MLB_INFO_Description
#define MODULE_INTERFACE_INFO_Version		MLB_INFO_Version
#define MODULE_INTERFACE_INFO_Menu_Path		MLB_INFO_Menu_Path

#define MODULE_INTERFACE_DLL_MAIN			MLB_INTERFACE
#define Get_Module_Info						Get_Info

//---------------------------------------------------------
// ::Get_Callback()								: gone
// ::SYS_Set_StringCopy							: gone, use CSG_String instead
// ::SYS_Add_StringCopy							: gone, use CSG_String instead
// CShapes::Get_Table()							: returns reference
// CTable::Update_Statistics()					: gone
// CColors::Set_Ramp()							: incompatible
// CModule_Grid::Get_Gradient()					: gone, use CGrid::Get_Gradient() instead
// CModule_Interactive::Execute_Finish()		: function has no arguments anymore
// CModule_Interactive::On_Set_Position()		: function has no 'callback' argument anymore
// CModule::Execute()							: function has no arguments anymore


///////////////////////////////////////////////////////////
//														 //
//				SAGA 2.0 Beta Compatibility				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define PFNC_Callback					
#define API_Set_Callback				SG_Set_Callback
#define API_Callback_					SG_Callback_
#define API_Create_						SG_Create_
#define API_Read_						SG_Read_
#define API_Write_						SG_Write_
#define API_Malloc						SG_Malloc
#define API_Calloc						SG_Calloc
#define API_Realloc						SG_Realloc
#define API_Free						SG_Free
#define API_Swap_Bytes					SG_Swap_Bytes
#define API_Directory_					SG_Directory_
#define API_DegreeStr2Double			SG_Degree_To_Double
#define API_Double2DegreeStr			SG_Double_To_Degree
#define API_DateStr2Double				SG_Date_To_Double
#define API_Double2DateStr				SG_Double_To_Date
#define API_Remove_File					SG_File_Delete
#define API_Extract_File_Name			SG_File_Get_Name
#define API_Extract_File_Path			SG_File_Get_Path
#define API_Make_File_Path				SG_File_Make_Path
#define API_Cmp_File_Extension			SG_File_Cmp_Extension
#define API_Get_						SG_Get_


//---------------------------------------------------------
#define CMAT_Spline						CSG_Spline
#define TMAT_Regression_Type			TSG_Regression_Type
#define CMAT_Regression					CSG_Regression
#define CMAT_Regression_Multiple		CSG_Regression_Multiple
#define CMAT_Grid_Radius				CSG_Grid_Radius
#define CMAT_Formula					CSG_Formula
#define CMAT_Trend						CSG_Trend

#define CAPI_String						CSG_String
#define CAPI_Strings					CSG_Strings
#define CTranslator						CSG_Translator
#define CColors							CSG_Colors
#define CHistory_Entry					CSG_History_Entry
#define CHistory						CSG_History

#define TGEO_Intersection				TSG_Intersection
#define TGEO_Point						TSG_Point
#define CGEO_Points						CSG_Points
#define TAPI_dPoint						TSG_Point
#define CAPI_dPoints					CSG_Points
#define TAPI_iPoint						TSG_Point_Int
#define CAPI_iPoint						CSG_Points_Int
#define TAPI_3D_dPoint					TSG_Point_3D
#define CAPI_3D_dPoints					CSG_Points_3D
#define TGEO_Rect						TSG_Rect
#define CGEO_Rect						CSG_Rect
#define CGEO_Rects						CSG_Rects

#define GEO_Get_Distance				SG_Get_Distance
#define GEO_Get_Angle_Of_Direction		SG_Get_Angle_Of_Direction
#define GEO_Get_Crossing				SG_Get_Crossing
#define GEO_Get_Crossing_InRegion		SG_Get_Crossing_InRegion
#define GEO_Get_Nearest_Point_On_Line	SG_Get_Nearest_Point_On_Line
#define GEO_Get_Triangle_CircumCircle	SG_Get_Triangle_CircumCircle
#define GEO_Get_Polygon_Area			SG_Get_Polygon_Area

#define CPDF_Document					CDoc_PDF
#define CHTML_Document					CDoc_HTML
#define CSVG_Graph						CDoc_SVG


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__compatibility_H
