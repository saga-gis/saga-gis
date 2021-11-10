
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
//                   VIEW_Histogram.h                    //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Histogram_H
#define _HEADER_INCLUDED__SAGA_GUI__VIEW_Histogram_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "view_base.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CVIEW_Histogram : public CVIEW_Base
{
public:
	CVIEW_Histogram(class CWKSP_Layer *pLayer);

	static class wxToolBarBase *	_Create_ToolBar		(void);
	static class wxMenu *			_Create_Menu		(void);

	virtual void					Do_Update			(void);

	virtual void					On_Command_UI		(wxUpdateUIEvent &event);


private:

	bool							m_bCumulative, m_bGaussian, m_bColored, m_bMouse_Down;

	int								m_XLabeling, m_Margin_Left, m_Margin_Bottom, m_Gaussian_Color, m_Gaussian_Size;

	wxPoint							m_Mouse_Down, m_Mouse_Move;

	class CWKSP_Layer				*m_pLayer;


	void							Draw				(wxDC &dc, wxRect r);
	void							Draw_Histogram		(wxDC &dc, wxRect r);
	void							Draw_Frame			(wxDC &dc, wxRect r);
	wxRect							Draw_Get_rDiagram	(wxRect r);

	void							On_Paint			(wxPaintEvent    &event);
	void							On_Size				(wxSizeEvent     &event);

	void							On_Mouse_Motion		(wxMouseEvent    &event);
	void							On_Mouse_LDown		(wxMouseEvent    &event);
	void							On_Mouse_LUp		(wxMouseEvent    &event);
	void							On_Mouse_RDown		(wxMouseEvent    &event);

	void							On_Properties		(wxCommandEvent  &event);
	void							On_Cumulative		(wxCommandEvent  &event);
	void							On_Gaussian			(wxCommandEvent  &event);
	void							On_Set_MinMax		(wxCommandEvent  &event);
	void							On_AsTable			(wxCommandEvent  &event);
	void							On_ToClipboard		(wxCommandEvent  &event);


	//-----------------------------------------------------
	DECLARE_CLASS(CVIEW_Histogram)
	DECLARE_EVENT_TABLE()

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Histogram_H
