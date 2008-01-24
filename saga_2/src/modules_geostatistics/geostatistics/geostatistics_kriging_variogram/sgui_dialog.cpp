
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     SAGA_GUI_API                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    sgui_dialog.cpp                    //
//                                                       //
//                 Copyright (C) 2008 by                 //
//                      Olaf Conrad                      //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    SAGA User Group Association            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/wxprec.h>

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#include <wx/settings.h>
#include <wx/dc.h>

#include <saga_api/saga_api.h>

//---------------------------------------------------------
#include "sgui_dialog.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CSGUI_Dialog, wxDialog)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSGUI_Dialog::CSGUI_Dialog(const wxString &Name)
	: wxDialog((wxWindow *)SG_UI_Get_Window_Main(), wxID_ANY, Name, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
{
	wxRect	r(0, 0, wxSystemSettings::GetMetric(wxSYS_SCREEN_X), wxSystemSettings::GetMetric(wxSYS_SCREEN_Y));
	r.Deflate((int)(0.1 * r.GetWidth()), (int)(0.1 * r.GetHeight()));
	SetSize(r);

	m_Ctrl_Color	= *wxBLACK;

	m_pSizer_Ctrl	= new wxStaticBoxSizer(wxVERTICAL, this, wxT(""));
	m_pSizer_Output	= new wxStaticBoxSizer(wxVERTICAL, this, wxT(""));

	wxSizer	*pSizer	= new wxBoxSizer(wxHORIZONTAL);

	pSizer->Add(m_pSizer_Output	, 1, wxALIGN_RIGHT|wxALL|wxEXPAND, SGUI_CTRL_SPACE);
	pSizer->Add(m_pSizer_Ctrl	, 0, wxALIGN_LEFT |wxALL|wxEXPAND, SGUI_CTRL_SPACE);

	pSizer->SetVirtualSizeHints(this);

	SetSizer(pSizer);
}

//---------------------------------------------------------
CSGUI_Dialog::~CSGUI_Dialog(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSGUI_Dialog::Add_Output(wxWindow *pOutput)
{
	m_pSizer_Output->Add(pOutput, 1, wxALL|wxEXPAND, SGUI_CTRL_SPACE);

	return( true );
}

//---------------------------------------------------------
bool CSGUI_Dialog::Add_Output(wxWindow *pOutput_A, wxWindow *pOutput_B, int Proportion_A, int Proportion_B)
{
	m_pSizer_Output->Add(pOutput_A, Proportion_A, wxALL|wxEXPAND, SGUI_CTRL_SPACE);
	m_pSizer_Output->Add(pOutput_B, Proportion_B, wxALL|wxEXPAND, SGUI_CTRL_SPACE);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSGUI_Dialog::Add_Spacer(int Space)
{
	m_pSizer_Ctrl->AddSpacer(Space);
}

//---------------------------------------------------------
wxButton * CSGUI_Dialog::Add_Button(const wxString &Name, int ID, const wxSize &Size)
{
	wxButton	*pButton	= new wxButton(this, ID, Name, wxDefaultPosition, Size);

	m_pSizer_Ctrl->Add(pButton, 0, wxALIGN_CENTER|wxALL|wxEXPAND, SGUI_CTRL_SMALLSPACE);

	return( pButton );
}

//---------------------------------------------------------
wxChoice * CSGUI_Dialog::Add_Choice(const wxString &Name, const wxArrayString &Choices, int iSelect, int ID)
{
	wxStaticText	*pLabel		= new wxStaticText(this, wxID_ANY, Name, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
	wxChoice		*pChoice	= new wxChoice(this, ID, wxDefaultPosition, wxDefaultSize, Choices);

	pLabel	->SetForegroundColour(m_Ctrl_Color);
	pChoice	->SetSelection(iSelect);

	m_pSizer_Ctrl->Add(pLabel , 0, wxALIGN_CENTER|wxLEFT|wxRIGHT|wxTOP   , SGUI_CTRL_SMALLSPACE);
	m_pSizer_Ctrl->Add(pChoice, 0, wxALIGN_CENTER|wxLEFT|wxRIGHT|wxBOTTOM, SGUI_CTRL_SMALLSPACE);

	return( pChoice );
}

//---------------------------------------------------------
wxCheckBox * CSGUI_Dialog::Add_CheckBox(const wxString &Name, bool bCheck, int ID)
{
	wxCheckBox		*pCheckBox	= new wxCheckBox(this, ID, Name, wxDefaultPosition, wxDefaultSize, 0);

	pCheckBox->SetForegroundColour(m_Ctrl_Color);
	pCheckBox->SetValue(bCheck);

	m_pSizer_Ctrl->Add(pCheckBox, 0, wxALIGN_LEFT|wxALL, SGUI_CTRL_SMALLSPACE);

	return( pCheckBox );
}

//---------------------------------------------------------
wxTextCtrl * CSGUI_Dialog::Add_TextCtrl(const wxString &Name, int Style, const wxString &Text, int ID)
{
	wxStaticText	*pLabel		= new wxStaticText(this, wxID_ANY, Name, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
	wxTextCtrl		*pTextCtrl	= new wxTextCtrl(this, ID, Text, wxDefaultPosition, wxSize(10, 100), Style);

	pLabel	->SetForegroundColour(m_Ctrl_Color);

	m_pSizer_Ctrl->Add(pLabel   , 0, wxALIGN_CENTER|wxLEFT|wxRIGHT|wxTOP   , SGUI_CTRL_SMALLSPACE);
	m_pSizer_Ctrl->Add(pTextCtrl, 1, wxALIGN_CENTER|wxLEFT|wxRIGHT|wxBOTTOM|wxEXPAND, SGUI_CTRL_SMALLSPACE);

	return( pTextCtrl );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
