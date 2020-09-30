
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
//                    VIEW_Layout.h                      //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Layout_H
#define _HEADER_INCLUDED__SAGA_GUI__VIEW_Layout_H


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
class CVIEW_Layout : public CVIEW_Base
{
public:
	CVIEW_Layout(class CVIEW_Layout_Info *pLayout);

	static class wxToolBarBase *	_Create_ToolBar		(void);
	static class wxMenu *			_Create_Menu		(void);

	virtual void					Do_Destroy			(void);
	virtual void					Do_Update			(void);

	void							Ruler_Set_Position	(int x, int y);
	void							Ruler_Refresh		(double xMin, double xMax, double yMin, double yMax);


private:

	class CVIEW_Ruler				*m_pRuler_X, *m_pRuler_Y;

	class CVIEW_Layout_Info			*m_pLayout;

	class CVIEW_Layout_Control		*m_pControl;


	void							On_Key_Event		(wxKeyEvent      &event);
	void							On_Size				(wxSizeEvent     &event);

	void							On_Load				(wxCommandEvent  &event);
	void							On_Save				(wxCommandEvent  &event);

	void							On_Properties		(wxCommandEvent  &event);

	void							On_Page_Setup		(wxCommandEvent  &event);
	void							On_Print_Setup		(wxCommandEvent  &event);
	void							On_Print_Preview	(wxCommandEvent  &event);
	void							On_Print			(wxCommandEvent  &event);
	void							On_Export			(wxCommandEvent  &event);

	void							On_Zoom				(wxCommandEvent  &event);

	void							On_Command			(wxCommandEvent  &event);
	void							On_Command_UI		(wxUpdateUIEvent &event);


	//-----------------------------------------------------
	DECLARE_CLASS(CVIEW_Layout)

	DECLARE_EVENT_TABLE()
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Layout_H
