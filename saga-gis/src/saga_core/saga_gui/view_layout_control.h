
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
//                 VIEW_Layout_Control.h                 //
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
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Layout_Control_H
#define _HEADER_INCLUDED__SAGA_GUI__VIEW_Layout_Control_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/scrolwin.h>
#include <wx/image.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CVIEW_Layout_Control : public wxScrolledWindow
{
public:
	CVIEW_Layout_Control(class CVIEW_Layout *pParent, class CVIEW_Layout_Info *pLayout);
	virtual ~CVIEW_Layout_Control(void);

	bool							Do_Destroy			(void);

	bool							Fit_To_Size			(int x, int y);

	bool							Set_Scrollbars		(void);

	virtual void					OnDraw				(wxDC &dc);


private:

	double							m_Zoom;

	wxPoint							m_Mouse_Down, m_Mouse_Move;

	wxImage							m_Image;

	class CVIEW_Layout_Info			*m_pLayout;


	bool							Set_Zoom			(double Zoom);
	void							Set_Zoom_Centered	(double Zooming, wxPoint Center);
	void							Set_Rulers			(void);

	void							On_Tracker_Changed	(wxCommandEvent  &event);

	void							On_Mouse_Event		(wxMouseEvent    &event);
	void							On_Mouse_Wheel		(wxMouseEvent    &event);

	void							On_Item_Menu		(wxCommandEvent  &event);
	void							On_Item_Menu_UI		(wxUpdateUIEvent &event);


	//-----------------------------------------------------
	DECLARE_EVENT_TABLE()
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Layout_Control_H
