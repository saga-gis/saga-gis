
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Histogram_H
#define _HEADER_INCLUDED__SAGA_GUI__VIEW_Histogram_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "view_base.h"

//---------------------------------------------------------
class CWKSP_Layer;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CVIEW_Histogram : public CVIEW_Base
{
public:
	CVIEW_Histogram(CWKSP_Layer *pLayer);
	virtual ~CVIEW_Histogram(void);

	static class wxToolBarBase *	_Create_ToolBar		(void);
	static class wxMenu *			_Create_Menu		(void);

	void							On_Size				(wxSizeEvent     &event);
	void							On_Paint			(wxPaintEvent    &event);

	void							On_Mouse_Motion		(wxMouseEvent    &event);
	void							On_Mouse_LDown		(wxMouseEvent    &event);
	void							On_Mouse_LUp		(wxMouseEvent    &event);
	void							On_Mouse_RDown		(wxMouseEvent    &event);

	virtual void					On_Command_UI		(wxUpdateUIEvent &event);

	void							On_Cumulative		(wxCommandEvent  &event);
	void							On_Cumulative_UI	(wxUpdateUIEvent &event);
	void							On_AsTable			(wxCommandEvent  &event);

	void							Draw				(wxDC &dc, wxRect rDraw);

	bool							Update_Histogram	(void);


private:

	bool							m_bCumulative, m_bMouse_Down;

	wxPoint							m_Mouse_Down, m_Mouse_Move;

	CWKSP_Layer						*m_pLayer;


	wxRect							_Draw_Get_rDiagram	(wxRect r);

	void							_Draw_Histogram		(wxDC &dc, wxRect r);
	void							_Draw_Frame			(wxDC &dc, wxRect r);


private:

	DECLARE_EVENT_TABLE()
	DECLARE_CLASS(CVIEW_Histogram)

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Histogram_H
