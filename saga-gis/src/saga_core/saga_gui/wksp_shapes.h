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
//                    WKSP_Shapes.h                      //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Shapes_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_Shapes_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "wksp_layer.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP_Shapes : public CWKSP_Layer
{
public:
	CWKSP_Shapes(CSG_Shapes *pShapes);
	virtual ~CWKSP_Shapes(void);

	virtual TWKSP_Item			Get_Type				(void)	{	return( WKSP_ITEM_Shapes );	}

	CSG_Shapes *				Get_Shapes				(void)	{	return( (CSG_Shapes *)m_pObject );	}
	class CWKSP_Table *			Get_Table				(void)	{	return( m_pTable );		}

	virtual wxString			Get_Description			(void);

	virtual wxMenu *			Get_Menu				(void);

	virtual bool				On_Command				(int Cmd_ID);
	virtual bool				On_Command_UI			(wxUpdateUIEvent &event);

	virtual wxString			Get_Value				(CSG_Point ptWorld, double Epsilon);
	virtual double				Get_Value_Range			(void);

	int							Get_Color_Field			(void)	{	return( m_iColor );		}
	int							Get_Label_Field			(void)	{	return( m_iLabel );		}

	wxString					Get_Name_Attribute		(void);

	bool						is_Editing				(void)	{	return( m_Edit_pShape != NULL );	}


protected:

	int							m_iColor, m_iLabel, m_iLabel_Size, m_Label_Prec, m_Label_Eff, m_iExtraInfo, m_Edit_iPart, m_Edit_iPoint, m_Chart_Type, m_Chart_sField, m_Chart_sType;

	double						m_Chart_sSize, m_Chart_sRange;

	wxColour					m_Def_Color, m_Edit_Color, m_Sel_Color, m_Label_Eff_Color;

	CSG_Points_Int				m_Chart;

	CSG_Parameters				m_Chart_Options;

	CSG_Shape					*m_Edit_pShape;

	CSG_Shapes					m_Edit_Shapes;

	class CWKSP_Table			*m_pTable;


	virtual void				On_Create_Parameters	(void);
	virtual void				On_DataObject_Changed	(void);
	virtual void				On_Parameters_Changed	(void);

	virtual int					On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags);

	virtual void				On_Update_Views			(void);

	virtual void				On_Draw					(CWKSP_Map_DC &dc_Map, bool bEdit);

	CSG_Parameter *				_AttributeList_Add		(CSG_Parameter *pNode, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description);
	void						_AttributeList_Set		(CSG_Parameter *pFields, bool bAddNoField);

	CSG_Parameter *				_BrushList_Add			(CSG_Parameter *pNode, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description);
	int							_BrushList_Get_Style	(int Index);

	CSG_Parameter *				_PenList_Add			(CSG_Parameter *pNode, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description);
	int							_PenList_Get_Style		(int Index);

	virtual void				_Draw_Initialize		(CWKSP_Map_DC &dc_Map)										= 0;
	virtual void				_Draw_Shape				(CWKSP_Map_DC &dc_Map, CSG_Shape *pShape, bool bSelection)	= 0;
	virtual void				_Draw_Label				(CWKSP_Map_DC &dc_Map, CSG_Shape *pShape)					= 0;

	bool						_Get_Class_Color		(CSG_Shape *pShape, int &Color);

	void						_Draw_Chart				(CWKSP_Map_DC &dc_Map, CSG_Shape *pShape);
	void						_Draw_Chart_Pie			(CWKSP_Map_DC &dc_Map, CSG_Table_Record *pRecord, bool bOutline, int x, int y, int size);
	void						_Draw_Chart_Bar			(CWKSP_Map_DC &dc_Map, CSG_Table_Record *pRecord, bool bOutline, int x, int y, int sx, int sy);

	bool						_Chart_is_Valid			(void)	{	return( m_Chart.Get_Count() > 0 );	}
	bool						_Chart_Set_Options		(void);
	bool						_Chart_Get_Options		(void);


	//-----------------------------------------------------
	// Editing...

	virtual wxMenu *			On_Edit_Get_Menu		(void);
	virtual TSG_Rect			On_Edit_Get_Extent		(void);
	virtual bool				On_Edit_Set_Attributes	(void);

	virtual bool				On_Edit_On_Key_Down		(int KeyCode);
	virtual bool				On_Edit_On_Mouse_Down	(CSG_Point Point, double ClientToWorld, int Key);
	virtual bool				On_Edit_On_Mouse_Up		(CSG_Point Point, double ClientToWorld, int Key);
	virtual bool				On_Edit_On_Mouse_Move	(wxWindow *pMap, CSG_Rect rWorld, wxPoint pt, wxPoint ptLast, int Key);

	void						_LUT_Create				(void);

	bool						_Edit_Set_Attributes	(void);

	bool						_Edit_Shape				(void);
	bool						_Edit_Shape_Start		(void);
	bool						_Edit_Shape_Stop		(void);
	bool						_Edit_Shape_Stop		(bool bSave);
	bool						_Edit_Shape_Add			(void);
	bool						_Edit_Shape_Del			(void);
	bool						_Edit_Part_Add			(void);
	bool						_Edit_Part_Del			(void);
	bool						_Edit_Point_Del			(void);

	void						_Edit_Shape_Draw_Point	(wxDC &dc, TSG_Point_Int Point, bool bSelected);
	void						_Edit_Shape_Draw_Point	(wxDC &dc, int x, int y, bool bSelected);
	virtual void				_Edit_Shape_Draw_Move	(wxDC &dc, CSG_Rect rWorld, double ClientToWorld, wxPoint Point);
	virtual void				_Edit_Shape_Draw		(CWKSP_Map_DC &dc_Map);

	virtual int					_Edit_Shape_HitTest		(CSG_Point Point, double max_Dist, int &iPart, int &iPoint);

	void						_Edit_Snap_Point		(CSG_Point &Point, double ClientToWorld);
	void						_Edit_Snap_Point		(CSG_Point Point, CSG_Point &snap_Point, double &snap_Dist, CSG_Shapes *pShapes, bool bLine);
	void						_Edit_Snap_Point		(CSG_Point Point, CSG_Point &snap_Point, double &snap_Dist, CSG_Shape *pShape);
	virtual void				_Edit_Snap_Point_ToLine (CSG_Point Point, CSG_Point &snap_Point, double &snap_Dist, CSG_Shape *pShape);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Shapes_H
