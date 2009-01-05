
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
// instead of the header:
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

#ifndef RGB
#define RGB									SG_GET_RGB
#endif

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
// ::Get_Callback()							: dropped
// ::SYS_Set_StringCopy						: dropped, use CSG_String instead
// ::SYS_Add_StringCopy						: dropped, use CSG_String instead
// CSG_Shapes::Get_Table()						: returns now reference instead of pointer
// CSG_Table::Update_Statistics()				: dropped
// CColors::Set_Ramp()						: incompatible
// CSG_Module_Grid::Get_Gradient()				: dropped, use CSG_Grid::Get_Gradient() instead
// CSG_Module_Interactive::Execute_Finish()	: function has no arguments anymore
// CSG_Module_Interactive::On_Set_Position()	: function has no 'callback' argument anymore
// CSG_Module::Execute()						: function has no arguments anymore


///////////////////////////////////////////////////////////
//														 //
//				SAGA 2.0 Beta Compatibility				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define PFNC_Callback					TSG_PFNC_UI_Callback
#define API_Set_Callback				SG_Set_UI_Callback
#define API_Get_Callback				SG_Get_UI_Callback
#define API_Create_Grid					SG_Create_Grid
#define API_Create_Shapes				SG_Create_Shapes
#define API_Create_Table				SG_Create_Table
#define API_Create_TIN					SG_Create_TIN
#define API_Malloc						SG_Malloc
#define API_Calloc						SG_Calloc
#define API_Realloc						SG_Realloc
#define API_Free						SG_Free
#define API_Swap_Bytes					SG_Swap_Bytes
#define API_DegreeStr2Double			SG_Degree_To_Double
#define API_Double2DegreeStr			SG_Double_To_Degree
#define API_DateStr2Double				SG_Date_To_Double
#define API_Double2DateStr				SG_Double_To_Date
#define API_Remove_File					SG_File_Delete
#define API_Extract_File_Name			SG_File_Get_Name
#define API_Extract_File_Path			SG_File_Get_Path
#define API_Make_File_Path				SG_File_Make_Path
#define API_Cmp_File_Extension			SG_File_Cmp_Extension

#define API_Callback_					SG_UI_Callback_
#define API_Directory_isValid			SG_Dir_isValid
#define API_Directory_Make				SG_Dir_Create
#define API_Read_						SG_Read_
#define API_Write_						SG_Write_
#define API_Get_						SG_Get_
#define SG_Dir_isValid					SG_Dir_Exists


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
#define COLOR_GET_						SG_GET_
#define COLOR_DEF_						SG_COLOR_
#define COLOR_PALETTE_					SG_COLORS_
#define CColors							CSG_Colors
#define CHistory_Entry					CSG_History_Entry
#define CHistory						CSG_History

#define TGEO_Intersection				TSG_Intersection
#define TGEO_Point						TSG_Point
#define CGEO_Point						CSG_Point
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

#define CPDF_Document					CSG_Doc_PDF
#define CHTML_Document					CSG_Doc_HTML
#define CSVG_Graph						CSG_Doc_SVG

//---------------------------------------------------------
#define CDataObject						CSG_Data_Object
#define CGrid							CSG_Grid
#define CGrid_System					CSG_Grid_System
#define CModule							CSG_Module
#define CModule_Grid_Interactive		CSG_Module_Grid_Interactive
#define CModule_Interactive				CSG_Module_Interactive
#define CModule_Interactive_Base		CSG_Module_Interactive_Base
#define CModule_Library_Interface		CSG_Module_Library_Interface
#define CShape							CSG_Shape
#define CShape_Line						CSG_Shape_Line
#define CShape_Point					CSG_Shape_Point
#define CShape_Points					CSG_Shape_Points
#define CShape_Polygon					CSG_Shape_Polygon
#define CShapes							CSG_Shapes
#define CShape_Search					CSG_Shape_Search
#define CTable							CSG_Table
#define CTable_DBase					CSG_Table_DBase
#define CTable_Record					CSG_Table_Record
#define CTable_Value					CSG_Table_Value
#define CTable_Value_Date				CSG_Table_Value_Date
#define CTable_Value_Double				CSG_Table_Value_Double
#define CTable_Value_Int				CSG_Table_Value_Int
#define CTable_Value_String				CSG_Table_Value_String
#define CTIN							CSG_TIN
#define CTIN_Edge						CSG_TIN_Edge
#define CTIN_Point						CSG_TIN_Node
#define CTIN_Triangle					CSG_TIN_Triangle
#define CParameters						CSG_Parameters
#define CParameter						CSG_Parameter
#define CParameter_Bool					CSG_Parameter_Bool
#define CParameter_Choice				CSG_Parameter_Choice
#define CParameter_Color				CSG_Parameter_Color
#define CParameter_Colors				CSG_Parameter_Colors
#define CParameter_Data					CSG_Parameter_Data
#define CParameter_DataObject			CSG_Parameter_Data_Object
#define CParameter_DataObject_Output	CSG_Parameter_Data_Object_Output
#define CParameter_Degree				CSG_Parameter_Degree
#define CParameter_Double				CSG_Parameter_Double
#define CParameter_FilePath				CSG_Parameter_File_Name
#define CParameter_FixedTable			CSG_Parameter_Fixed_Table
#define CParameter_Font					CSG_Parameter_Font
#define CParameter_Grid					CSG_Parameter_Grid
#define CParameter_Grid_List			CSG_Parameter_Grid_List
#define CParameter_Grid_System			CSG_Parameter_Grid_System
#define CParameter_Int					CSG_Parameter_Int
#define CParameter_List					CSG_Parameter_List
#define CParameter_Node					CSG_Parameter_Node
#define CParameter_Parameters			CSG_Parameter_Parameters
#define CParameter_Range				CSG_Parameter_Range
#define CParameter_Shapes				CSG_Parameter_Shapes
#define CParameter_Shapes_List			CSG_Parameter_Shapes_List
#define CParameter_String				CSG_Parameter_String
#define CParameter_Table				CSG_Parameter_Table
#define CParameter_Table_Field			CSG_Parameter_Table_Field
#define CParameter_Table_List			CSG_Parameter_Table_List
#define CParameter_Text					CSG_Parameter_Text
#define CParameter_TIN					CSG_Parameter_TIN
#define CParameter_TIN_List				CSG_Parameter_TIN_List
#define CParameter_Value				CSG_Parameter_Value

#define TDataObject_Type				TSG_Data_Object_Type
#define TGrid_Type						TSG_Grid_Type
#define TGrid_Memory_Type				TSG_Grid_Memory_Type
#define TGrid_File_Format				TSG_Grid_File_Format
#define TGrid_File_Key					TSG_Grid_File_Key
#define TGrid_Interpolation				TSG_Grid_Interpolation
#define TGrid_Operation					TSG_Grid_Operation
#define TShape_Type						TSG_Shape_Type
#define TSVG_Alignment					TSG_SVG_Alignment
#define TTable_FieldType				TSG_Table_Field_Type
#define TTable_FileType					TSG_Table_File_Type
#define TTable_Index_Order				TSG_Table_Index_Order
#define TPDF_Font_Type					TSG_PDF_Font_Type
#define TPDF_Page_Size					TSG_PDF_Page_Size
#define TPDF_Title_Level				TSG_PDF_Title_Level
#define TParameter_Type					TSG_Parameter_Type
#define TModule_Type					TSG_Module_Type
#define TModule_Interactive_Mode		TSG_Module_Interactive_Mode
#define TModule_Interactive_DragMode	TSG_Module_Interactive_DragMode
#define TModule_Error					TSG_Module_Error
#define TMLB_Info						TSG_MLB_Info

#define Add_Extra_Parameters			Add_Parameters
#define Get_Extra_Parameters			Get_Parameters
#define Dlg_Extra_Parameters			Dlg_Parameters

#define TABLE_INDEX_Up					TABLE_INDEX_Ascending
#define TABLE_INDEX_Down				TABLE_INDEX_Descending


///////////////////////////////////////////////////////////
//														 //
//				SAGA 2.0.3 Compatibility				 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// CSG_Grid

#define Sort_Discard					Set_Index


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__compatibility_H
