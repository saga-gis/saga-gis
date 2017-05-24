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
//                 ACTIVE_Parameters.h                   //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__ACTIVE_Parameters_H
#define _HEADER_INCLUDED__SAGA_GUI__ACTIVE_Parameters_H


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
class CACTIVE_Parameters : public wxPanel
{
	DECLARE_CLASS(CACTIVE_Parameters)

public:
	CACTIVE_Parameters(wxWindow *pParent);
	virtual ~CACTIVE_Parameters(void);

	void						On_Size				(wxSizeEvent     &event);
	void						On_Key				(wxKeyEvent      &event);

	void						On_Apply			(wxCommandEvent  &event);
	void						On_Apply_UI			(wxUpdateUIEvent &event);
	void						On_Restore			(wxCommandEvent  &event);
	void						On_Restore_UI		(wxUpdateUIEvent &event);
	void						On_Execute			(wxCommandEvent  &event);
	void						On_Load				(wxCommandEvent  &event);
	void						On_Save				(wxCommandEvent  &event);

	bool						Set_Parameters		(class CWKSP_Base_Item *pItem);
	bool						Update_Parameters	(class CSG_Parameters *pParameters, bool bSave);
	bool						Update_DataObjects	(void);
	bool						Restore_Parameters	(void);


private:

	int							m_btn_height;

	class wxButton				*m_Btn_Restore, *m_Btn_Apply, *m_Btn_Execute, *m_Btn_Load, *m_Btn_Save;

	class CWKSP_Base_Item		*m_pItem;

	class CParameters_Control	*m_pControl;


	void						_Set_Positions		(void);


//---------------------------------------------------------
DECLARE_EVENT_TABLE()
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__ACTIVE_Parameters_H
