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
//                    dlg_choices.h                      //
//                                                       //
//          Copyright (C) 2017 by Olaf Conrad            //
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
//                University of Hamburg                  //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__dlg_choices_H
#define _HEADER_INCLUDED__SAGA_GUI__dlg_choices_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/button.h>
#include <wx/listbox.h>

#include "dlg_base.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CDLG_Choices : public CDLG_Base
{
	DECLARE_CLASS(CDLG_Choices)
	DECLARE_EVENT_TABLE()

public:
	CDLG_Choices(class CSG_Parameter_Choices *Parameter, wxString Caption);

	void						On_DClick_Add	(wxCommandEvent &event);
	void						On_Add			(wxCommandEvent &event);
	void						On_Add_All		(wxCommandEvent &event);
	void						On_DClick_Del	(wxCommandEvent &event);
	void						On_Del			(wxCommandEvent &event);
	void						On_Del_All		(wxCommandEvent &event);
	void						On_Up			(wxCommandEvent &event);
	void						On_Down			(wxCommandEvent &event);


protected:

	wxListBox					*m_pChoices, *m_pSelection;

	CSG_Parameter_Choices		*m_pParameter;


	virtual void				Set_Position	(wxRect r);

	virtual void				Save_Changes	(void);


private:

	int							m_Btn_Height;

	CSG_Array_Int				m_Index;

	wxButton					*m_pBtn_Add, *m_pBtn_Add_All, *m_pBtn_Del, *m_pBtn_Del_All, *m_pBtn_Up, *m_pBtn_Down;


	static int					_Compare_Up		(int *first, int *second);
	static int					_Compare_Down	(int *first, int *second);

	void						_Add			(void);
	void						_Del			(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__dlg_choices_H
