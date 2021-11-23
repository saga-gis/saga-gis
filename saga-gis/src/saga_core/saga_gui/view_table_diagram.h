
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
//                 VIEW_Table_Diagram.h                  //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Table_Diagram_H
#define _HEADER_INCLUDED__SAGA_GUI__VIEW_Table_Diagram_H


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
class CVIEW_Table_Diagram : public CVIEW_Base
{
public:
	CVIEW_Table_Diagram(class CWKSP_Table *pTable);

	static class wxToolBarBase *		_Create_ToolBar		(void);
	static class wxMenu *				_Create_Menu		(void);

	virtual void						Do_Update			(void);

	virtual void						On_Command_UI		(wxUpdateUIEvent &event);


private:

	class CVIEW_Table_Diagram_Control	*m_pControl;


	void								On_Size				(wxSizeEvent    &event);

	void								On_Parameters		(wxCommandEvent &event);
	void								On_Size_Fit			(wxCommandEvent &event);
	void								On_Size_Inc			(wxCommandEvent &event);
	void								On_Size_Dec			(wxCommandEvent &event);
	void								On_Legend			(wxCommandEvent &event);
	void								On_SaveToClipboard	(wxCommandEvent &event);
	void								On_Key_Down			(wxKeyEvent     &event);


	//-----------------------------------------------------
	DECLARE_CLASS(CVIEW_Table_Diagram)
	DECLARE_EVENT_TABLE()

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__VIEW_Table_Diagram_H
