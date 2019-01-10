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
enum
{
	EDIT_SHAPE_MODE_Normal	= 0,
	EDIT_SHAPE_MODE_Split,
	EDIT_SHAPE_MODE_Move
};


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
	virtual double				Get_Value_Minimum		(void);
	virtual double				Get_Value_Maximum		(void);
	virtual double				Get_Value_Range			(void);
	virtual double				Get_Value_Mean			(void);
	virtual double				Get_Value_StdDev		(void);

	int							Get_Field_Value			(void)	{	return( m_fValue  );	}
	int							Get_Field_Normal		(void)	{	return( m_fNormal );	}
	int							Get_Field_Label			(void)	{	return( m_fLabel  );	}

	wxString					Get_Name_Attribute		(void);

	bool						is_Editing				(void)	{	return( m_Edit_pShape != NULL );	}

	virtual wxMenu *			Edit_Get_Menu			(void);
	virtual TSG_Rect			Edit_Get_Extent			(void);
	virtual bool				Edit_On_Key_Down		(int KeyCode);
	virtual bool				Edit_On_Mouse_Down		(CSG_Point Point, double ClientToWorld, int Key);
	virtual bool				Edit_On_Mouse_Up		(CSG_Point Point, double ClientToWorld, int Key);
	virtual bool				Edit_On_Mouse_Move		(wxWindow *pMap, CSG_Rect rWorld, wxPoint pt, wxPoint ptLast, int Key);
	virtual bool				Edit_Set_Index			(int Index);
	virtual bool				Edit_Set_Attributes		(void);


protected:

	bool						m_bNoData, m_Edit_bGleam;

	int							m_fValue, m_fNormal, m_bVertices,
								m_fLabel, m_Label_Prec, m_Label_Eff, m_Label_Eff_Size,
								m_Edit_Mode, m_Edit_iPart, m_Edit_iPoint;

	wxColour					m_Edit_Color, m_Sel_Color, m_Label_Eff_Color;

	CSG_Simple_Statistics		m_Metrics;

	CSG_Shape					*m_Edit_pShape;

	class CWKSP_Table			*m_pTable;


	bool						Set_Metrics				(int zField, int nField);

	virtual void				On_Create_Parameters	(void);
	virtual void				On_DataObject_Changed	(void);
	virtual void				On_Parameters_Changed	(void);

	virtual int					On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags);

	virtual void				On_Update_Views			(void);

	virtual void				On_Draw					(CWKSP_Map_DC &dc_Map, int Flags);

	bool						Get_Class_Color			(CSG_Shape *pShape, int &Color);

	void						AttributeList_Set		(CSG_Parameter *pFields, bool bAddNoField);

	CSG_Parameter *				BrushList_Add			(const CSG_String &ParentID, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description);
	wxBrushStyle				BrushList_Get_Style		(const CSG_String &Identifier);

	CSG_Parameter *				PenList_Add				(const CSG_String &ParentID, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description);
	int							PenList_Get_Style		(const CSG_String &Identifier);

	virtual void				Draw_Initialize			(CWKSP_Map_DC &dc_Map, int Flags)												= 0;
	virtual void				Draw_Shape				(CWKSP_Map_DC &dc_Map, CSG_Shape *pShape, int Selection)			= 0;
	virtual void				Draw_Label				(CWKSP_Map_DC &dc_Map, CSG_Shape *pShape, const wxString &Label)	= 0;

	virtual void				Edit_Shape_Draw_Move	(wxDC &dc, const CSG_Rect &rWorld, const wxPoint &Point, const TSG_Point &ptWorld);
	virtual void				Edit_Shape_Draw_Move	(wxDC &dc, const CSG_Rect &rWorld, const wxPoint &Point);
	virtual void				Edit_Shape_Draw			(CWKSP_Map_DC &dc_Map);
	virtual int					Edit_Shape_HitTest		(CSG_Point Point, double max_Dist, int &iPart, int &iPoint);
	virtual void				Edit_Snap_Point_ToLine	(CSG_Point Point, CSG_Point &snap_Point, double &snap_Dist, CSG_Shape *pShape);


private:

	void						_LUT_Create				(void);
	void						_LUT_Import				(void);

	void						_Draw_Shape				(CWKSP_Map_DC &dc_Map, CSG_Shape *pShape, int Selection = 0);
	void						_Draw_Label				(CWKSP_Map_DC &dc_Map, CSG_Shape *pShape, int PointSize = 0);


	//-----------------------------------------------------
	// Charts...

	int							m_Chart_Type, m_Chart_sField, m_Chart_sType;

	double						m_Chart_sSize, m_Chart_sRange;

	CSG_Points_Int				m_Chart;

	CSG_Parameters				m_Chart_Options;


	bool						_Chart_is_Valid			(void)	{	return( m_Chart.Get_Count() > 0 );	}
	bool						_Chart_Set_Options		(void);
	bool						_Chart_Get_Options		(void);

	void						_Draw_Chart				(CWKSP_Map_DC &dc_Map, CSG_Shape *pShape);
	void						_Draw_Chart_Pie			(CWKSP_Map_DC &dc_Map, CSG_Table_Record *pRecord, bool bOutline, int x, int y, int size);
	void						_Draw_Chart_Bar			(CWKSP_Map_DC &dc_Map, CSG_Table_Record *pRecord, bool bOutline, int x, int y, int sx, int sy);


	//-----------------------------------------------------
	// Editing...

	CSG_Shapes					m_Edit_Shapes;


	bool						_Edit_Shape				(void);
	bool						_Edit_Shape_Start		(void);
	bool						_Edit_Shape_Stop		(void);
	bool						_Edit_Shape_Stop		(bool bSave);
	bool						_Edit_Shape_Add			(void);
	bool						_Edit_Shape_Del			(void);
	bool						_Edit_Part_Add			(void);
	bool						_Edit_Part_Del			(void);
	bool						_Edit_Point_Del			(void);
	bool						_Edit_Merge				(void);
	bool						_Edit_Split				(void);
	bool						_Edit_Move				(bool bToggle = true);

	void						_Edit_Shape_Draw_Point	(wxDC &dc, TSG_Point_Int Point, bool bSelected);
	void						_Edit_Shape_Draw_Point	(wxDC &dc, int x, int y, bool bSelected);

	void						_Edit_Snap_Point		(CSG_Point &Point, double ClientToWorld);
	void						_Edit_Snap_Point		(CSG_Point Point, CSG_Point &snap_Point, double &snap_Dist, CSG_Shapes *pShapes, bool bLine);
	void						_Edit_Snap_Point		(CSG_Point Point, CSG_Point &snap_Point, double &snap_Dist, CSG_Shape *pShape);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Shapes_H
