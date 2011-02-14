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
//                    DLG_Text.cpp                       //
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
#include <wx/panel.h>
#include <wx/textctrl.h>

#include "res_controls.h"
#include "res_dialogs.h"

#include "helper.h"

#include "dlg_text.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CDLG_Text, CDLG_Base)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CDLG_Text, CDLG_Base)
	EVT_BUTTON			(ID_BTN_LOAD	, CDLG_Text::On_Load)
	EVT_BUTTON			(ID_BTN_SAVE	, CDLG_Text::On_Save)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDLG_Text::CDLG_Text(wxString *_pText, wxString Caption)
	: CDLG_Base(-1, Caption)
{
	m_pText		= _pText;

	m_pControl	= new wxTextCtrl(this, -1, *m_pText, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxSUNKEN_BORDER);

	Add_Button(ID_BTN_LOAD);
	Add_Button(ID_BTN_SAVE);

	Set_Positions();
}

//---------------------------------------------------------
CDLG_Text::~CDLG_Text(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Text::Save_Changes(void)
{
	if( m_pText )
	{
		*m_pText	= m_pControl->GetValue();
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Text::Set_Position(wxRect r)
{
	m_pControl->SetSize(r);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Text::On_Load(wxCommandEvent &event)
{
	wxString	File_Path;

	if( DLG_Open(File_Path, ID_DLG_TEXT_OPEN) )
	{
		m_pControl->LoadFile(File_Path);
	}
}

//---------------------------------------------------------
void CDLG_Text::On_Save(wxCommandEvent &event)
{
	wxString	File_Path;

	if( DLG_Save(File_Path, ID_DLG_TEXT_SAVE) )
	{
		m_pControl->SaveFile(File_Path);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
