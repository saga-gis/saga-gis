
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Shapes_Point_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_Shapes_Point_H


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "wksp_shapes.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP_Shapes_Point : public CWKSP_Shapes
{
public:
	CWKSP_Shapes_Point(CSG_Shapes *pShapes);

	bool						Get_Outline				(void)	{	return( m_bOutline );	}
	wxPen						Get_Def_Pen				(void)	{	return( m_Pen      );	}
	wxBrush						Get_Def_Brush			(void)	{	return( m_Brush    );	}

	bool						Get_Style_Size			(int &minSize, int &maxSize, double &Minimum, double &Scale, wxString *pName);

	void						Draw_Symbol				(wxDC &dc, int x, int y, int size);


protected:

	bool						m_bOutline;

	wxColour					m_Sel_Color_Fill;

	wxPen						m_Pen;

	wxBrush						m_Brush;

	wxImage						m_Symbol_Image;

	struct SSize                { int Unit, Field, Adjust; double Minimum, Offset, Scale;    } m_Size;

	struct SLabel               { int Align, Angle_Field; double Angle; CSG_Point Offset;    } m_Label;

	struct SSymbol              { int Type; wxImage Image;                                   } m_Symbol;

	struct SImage               { int Field, Align, Fit; double Offset, Scale;               } m_Image;

	struct SArrow               { int Field, Style, Width, Orientation, Unit; double Offset; } m_Arrow;

	struct SBeachball           { int Strike, Dip, Rake;                                     } m_Beachball;


	virtual void				On_Create_Parameters	(void);
	virtual void				On_DataObject_Changed	(void);
	virtual void				On_Parameters_Changed	(void);

	virtual int					On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags);

	virtual void				Draw_Initialize			(CSG_Map_DC &dc_Map, int Flags);
	virtual bool				Draw_Initialize			(CSG_Map_DC &dc_Map, int &Size, CSG_Shape *pShape, int Selection);
	virtual void				Draw_Shape				(CSG_Map_DC &dc_Map, CSG_Shape *pShape, int Selection);
	virtual void				Draw_Label				(CSG_Map_DC &dc_Map, CSG_Shape *pShape, const wxString &Label);

	void						Draw_Symbol				(CSG_Map_DC &dc, int x, int y, int size);


private:

	void						_Image_Draw				(CSG_Map_DC &dc, int x, int y, int size, const wxString &file);

	void						_Arrow_Draw				(CSG_Map_DC &dc_Map, int x, int y, int size, double direction);

	void						_Beachball_Draw			(CSG_Map_DC &dc, int x, int y, int size, double strike, double dip, double rake);
	void						_Beachball_Get_Plane	(CSG_Shape *pPlane, CSG_Shape *pCircle, const CSG_Vector &Normal);
	void						_Beachball_Get_Scaled	(CSG_Shape *pShape, double x, double y, double size);

};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Shapes_Point_H
