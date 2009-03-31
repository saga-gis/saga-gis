
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
#include <wx/scrolwin.h>

#include "view_base.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CVIEW_Histogram_Control : public wxScrolledWindow
{
public:
	CVIEW_Histogram_Control(wxWindow *pParent, class CWKSP_Layer *pLayer);

	bool							Update_Histogram	(void);

	bool							Get_Cumulative		(void)	{	return( m_bCumulative );	}
	void							Set_Cumulative		(bool bOn);


private:

	bool							m_bCumulative, m_bMouse_Down;

	wxPoint							m_Mouse_Down, m_Mouse_Move;

	class CWKSP_Layer				*m_pLayer;


	void							On_Mouse_Motion		(wxMouseEvent    &event);
	void							On_Mouse_LDown		(wxMouseEvent    &event);
	void							On_Mouse_LUp		(wxMouseEvent    &event);
	void							On_Mouse_RDown		(wxMouseEvent    &event);

	void							On_Size				(wxSizeEvent     &event);
	void							On_Paint			(wxPaintEvent    &event);

	void							_Draw				(wxDC &dc, wxRect rDraw);
	void							_Draw_Histogram		(wxDC &dc, wxRect r);
	void							_Draw_Frame			(wxDC &dc, wxRect r);
	wxRect							_Draw_Get_rDiagram	(wxRect r);


private:

	DECLARE_EVENT_TABLE()
	DECLARE_CLASS(CVIEW_Histogram_Control)

};


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
	virtual ~CVIEW_Histogram(void);

	static class wxToolBarBase *	_Create_ToolBar		(void);
	static class wxMenu *			_Create_Menu		(void);

	virtual void					On_Command_UI		(wxUpdateUIEvent &event);

	void							On_Cumulative		(wxCommandEvent  &event);
	void							On_Cumulative_UI	(wxUpdateUIEvent &event);
	void							On_AsTable			(wxCommandEvent  &event);

	bool							Update_Histogram	(void);


private:

	class CWKSP_Layer				*m_pLayer;

	CVIEW_Histogram_Control			*m_pControl;


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
