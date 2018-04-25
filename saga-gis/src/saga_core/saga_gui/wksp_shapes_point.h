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
//                 WKSP_Shapes_Point.h                   //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Shapes_Point_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_Shapes_Point_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "wksp_shapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP_Shapes_Point : public CWKSP_Shapes
{
public:
	CWKSP_Shapes_Point(CSG_Shapes *pShapes);

	bool						Get_Outline				(void)	{	return( m_bOutline );	}
	wxPen						Get_Def_Pen				(void)	{	return( m_Pen      );	}
	wxBrush						Get_Def_Brush			(void)	{	return( m_Brush    );	}

	bool						Get_Style_Size			(int &min_Size, int &max_Size, double &min_Value, double &dValue, wxString *pName);

	void						Draw_Symbol				(wxDC &dc, int x, int y, int size);


protected:

	bool						m_bOutline;

	int							m_iSize, m_Size_Type, m_Size_Scale, m_Symbol_Type, m_iLabel_Angle, m_Label_Align, m_Beachball[3];

	double						m_Size, m_dSize, m_Size_Min, m_Label_Angle;

	wxColour					m_Sel_Color_Fill;

	wxPen						m_Pen;

	wxBrush						m_Brush;

	wxImage						m_Symbol;


	virtual void				On_Create_Parameters	(void);
	virtual void				On_DataObject_Changed	(void);
	virtual void				On_Parameters_Changed	(void);

	virtual int					On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags);

	virtual void				Draw_Initialize			(CWKSP_Map_DC &dc_Map);
	virtual bool				Draw_Initialize			(CWKSP_Map_DC &dc_Map, int &Size, CSG_Shape *pShape, int Selection);
	virtual void				Draw_Shape				(CWKSP_Map_DC &dc_Map, CSG_Shape *pShape, int Selection);
	virtual void				Draw_Label				(CWKSP_Map_DC &dc_Map, CSG_Shape *pShape, const wxString &Label);


private:

	void						_Beachball_Draw			(wxDC &dc_Map, int x, int y, int size, double strike, double dip, double rake);
	void						_Beachball_Get_Plane	(CSG_Shape *pPlane, CSG_Shape *pCircle, const CSG_Vector &Normal);
	void						_Beachball_Get_Scaled	(CSG_Shape *pShape, double x, double y, double size);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Shapes_Point_H
