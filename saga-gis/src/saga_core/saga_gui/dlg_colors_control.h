
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
//                 DLG_Colors_Control.h                  //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__DLG_Colors_Control_H
#define _HEADER_INCLUDED__SAGA_GUI__DLG_Colors_Control_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/panel.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CDLG_Colors_Control : public wxPanel
{
	DECLARE_CLASS(CDLG_Colors_Control)

public:
	CDLG_Colors_Control(wxWindow *pParent, class CSG_Colors *pColors);
	virtual ~CDLG_Colors_Control(void);

	void						On_Size				(wxSizeEvent  &event);
	void						On_Paint			(wxPaintEvent &event);

	void						On_Mouse_LDown		(wxMouseEvent &event);
	void						On_Mouse_Motion		(wxMouseEvent &event);
	void						On_Mouse_LUp		(wxMouseEvent &event);
	void						On_Mouse_RUp		(wxMouseEvent &event);

	void						Set_Positions		(void);


protected:

	int							m_selBox;

	wxPoint						m_Mouse_Down, m_Mouse_Move;

	wxRect						m_red, m_green, m_blue, m_sum, m_rgb, m_grad;

	class CSG_Colors			*m_pColors;


	void						Draw_Box			(wxDC &dc, int BoxID);

	wxPoint						Get_ColorPosition	(wxPoint Point       , int BoxID);
	void						Set_Colors			(wxPoint A, wxPoint B, int BoxID);

	int							Get_SelBox			(const wxPoint &Point);
	bool						IsInBoxRect			(const wxPoint &Point, int BoxID);
	void						KeepInBoxRect		(      wxPoint &Point, int BoxID);
	void						KeepInBoxRect		(      wxPoint &Point, const wxRect &r);
	wxRect						Get_BoxRect			(                      int BoxID);


	//-----------------------------------------------------
	DECLARE_EVENT_TABLE()
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__DLG_Colors_Control_H
