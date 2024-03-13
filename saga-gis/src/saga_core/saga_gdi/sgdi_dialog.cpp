
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_GDI                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    sgui_dialog.cpp                    //
//                                                       //
//                 Copyright (C) 2009 by                 //
//                      Olaf Conrad                      //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/wxprec.h>

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#include <wx/settings.h>
#include <wx/dc.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/scrolwin.h>

#include <saga_api/saga_api.h>

#include "sgdi_dialog.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CSGDI_Dialog, wxDialog)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSGDI_Dialog::CSGDI_Dialog(const wxString &Name, int Style)
	: wxDialog((wxWindow *)SG_UI_Get_Window_Main(), wxID_ANY, Name, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
{
	if( Style & SGDI_DLG_STYLE_START_MAXIMISED )
	{
		Maximize();
	}

	m_Ctrl_Color    = *wxBLACK;

	m_pOutput_Sizer = new wxStaticBoxSizer(wxVERTICAL, this);

	wxSizer	*pSizer = new wxBoxSizer(wxHORIZONTAL);

	m_pCtrl = new wxScrolledWindow(this);//, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_STATIC);

	if( Style & SGDI_DLG_STYLE_CTRLS_RIGHT )
	{
		pSizer->Add(m_pOutput_Sizer, 1, wxALL|wxEXPAND, SGDI_CTRL_SPACE);
		pSizer->Add(m_pCtrl        , 0, wxALL|wxEXPAND, SGDI_CTRL_SPACE);
	}
	else
	{
		pSizer->Add(m_pCtrl        , 0, wxALL|wxEXPAND, SGDI_CTRL_SPACE);
		pSizer->Add(m_pOutput_Sizer, 1, wxALL|wxEXPAND, SGDI_CTRL_SPACE);
	}

	pSizer->FitInside(this);

	SetSizer(pSizer);

	m_pCtrl_Sizer = new wxBoxSizer(wxVERTICAL);
//	m_pCtrl_Sizer->FitInside(m_pCtrl);
	m_pCtrl->SetSizer(m_pCtrl_Sizer);
	m_pCtrl->SetScrollRate(0, 20);
}

//---------------------------------------------------------
CSGDI_Dialog::~CSGDI_Dialog(void)
{}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSGDI_Dialog::ShowModal(void)
{
#ifdef __WXMAC__
	Maximize();
#else
	if( GetParent()->IsShownOnScreen() )
	{
		wxRect r(GetParent()->GetScreenRect());
		r.Deflate((int)(0.1 * r.GetWidth()), (int)(0.1 * r.GetHeight()));
		SetSize(r);
	}
	#endif

	return( wxDialog::ShowModal() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSGDI_Dialog::Add_Output(wxWindow *pOutput, bool bReparent)
{
	if( bReparent ) // wxStaticBoxSizer items should be created as children of its wxStaticBox and not of wxDialog (except wxPanel objects?!)
	{
		pOutput->Reparent((wxWindow *)m_pOutput_Sizer->GetStaticBox());
	}

	m_pOutput_Sizer->Add(pOutput, 1, wxALL|wxEXPAND, SGDI_CTRL_SPACE);

	return( true );
}

//---------------------------------------------------------
bool CSGDI_Dialog::Add_Output(wxWindow *pOutput_A, wxWindow *pOutput_B, int Proportion_A, int Proportion_B, bool bReparent)
{
	if( bReparent ) // wxStaticBoxSizer items should be created as children of its wxStaticBox and not of wxDialog (except wxPanel objects?!)
	{
		pOutput_A->Reparent((wxWindow *)m_pOutput_Sizer->GetStaticBox());
		pOutput_B->Reparent((wxWindow *)m_pOutput_Sizer->GetStaticBox());
	}

	m_pOutput_Sizer->Add(pOutput_A, Proportion_A, wxALL|wxEXPAND, SGDI_CTRL_SPACE);
	m_pOutput_Sizer->Add(pOutput_B, Proportion_B, wxALL|wxEXPAND, SGDI_CTRL_SPACE);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSGDI_Dialog::Add_Spacer(int Space)
{
	m_pCtrl_Sizer->AddSpacer(Space);

	m_pCtrl->SetVirtualSize(m_pCtrl_Sizer->GetSize());
}

//---------------------------------------------------------
wxStaticText * CSGDI_Dialog::Add_Label(const wxString &Name, bool bCenter, int ID)
{
	if( Name.IsEmpty() )
	{
		return( NULL );
	}

	wxStaticText *pLabel = new wxStaticText(m_pCtrl, ID, Name, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);

	pLabel->SetForegroundColour(m_Ctrl_Color);

	m_pCtrl_Sizer->Add(pLabel, 0, wxLEFT|wxRIGHT|wxTOP|(bCenter ? (int)wxALIGN_CENTER : (int)wxEXPAND), SGDI_CTRL_SMALLSPACE);

	m_pCtrl->SetVirtualSize(m_pCtrl_Sizer->GetSize());

	return( pLabel );
}

//---------------------------------------------------------
wxButton * CSGDI_Dialog::Add_Button(const wxString &Name, int ID, const wxSize &Size)
{
	wxButton *pButton = new wxButton(m_pCtrl, ID, Name, wxDefaultPosition, Size);

	m_pCtrl_Sizer->Add(pButton, 0, wxALL|wxEXPAND, SGDI_CTRL_SMALLSPACE);

	m_pCtrl->SetVirtualSize(m_pCtrl_Sizer->GetSize());

	return( pButton );
}

//---------------------------------------------------------
wxChoice * CSGDI_Dialog::Add_Choice(const wxString &Name, const wxArrayString &Choices, int iSelect, int ID)
{
	Add_Label(Name, false);

	wxChoice *pChoice = new wxChoice(m_pCtrl, ID, wxDefaultPosition, wxDefaultSize, Choices);

	pChoice->SetSelection(iSelect);

	m_pCtrl_Sizer->Add(pChoice, 0, wxLEFT|wxRIGHT|wxBOTTOM|wxEXPAND, SGDI_CTRL_SMALLSPACE);

	m_pCtrl->SetVirtualSize(m_pCtrl_Sizer->GetSize());

	return( pChoice );
}

//---------------------------------------------------------
wxCheckBox * CSGDI_Dialog::Add_CheckBox(const wxString &Name, bool bCheck, int ID)
{
	wxCheckBox *pCheckBox = new wxCheckBox(m_pCtrl, ID, Name, wxDefaultPosition, wxDefaultSize, 0);

	pCheckBox->SetForegroundColour(m_Ctrl_Color);
	pCheckBox->SetValue(bCheck);

	m_pCtrl_Sizer->Add(pCheckBox, 0, wxALIGN_LEFT|wxALL, SGDI_CTRL_SMALLSPACE);

	m_pCtrl->SetVirtualSize(m_pCtrl_Sizer->GetSize());

	return( pCheckBox );
}

//---------------------------------------------------------
wxTextCtrl * CSGDI_Dialog::Add_TextCtrl(const wxString &Name, int Style, const wxString &Text, int ID)
{
	Add_Label(Name, true);

	int Stretch = Style & wxTE_MULTILINE;

	wxTextCtrl *pTextCtrl = new wxTextCtrl(m_pCtrl, ID, Text, wxDefaultPosition, SGDI_BTN_SIZE, Style);

	m_pCtrl_Sizer->Add(pTextCtrl, Stretch, wxLEFT|wxRIGHT|wxBOTTOM|wxEXPAND, SGDI_CTRL_SMALLSPACE);

	if( Style & wxTE_READONLY )
	{
		pTextCtrl->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	}

	m_pCtrl->SetVirtualSize(m_pCtrl_Sizer->GetSize());

	return( pTextCtrl );
}

//-----------------------------------------------------------------------------
CSGDI_Slider * CSGDI_Dialog::Add_Slider(const wxString &Name, double Value, double minValue, double maxValue, bool bValueAsPercent, int ID, int Width)
{
	Add_Label(Name, true);

	if( bValueAsPercent && maxValue > minValue )
	{
		Value = minValue + Value * (maxValue - minValue) / 100.0;
	}

	CSGDI_Slider *pSlider = new CSGDI_Slider(m_pCtrl, ID, Value, minValue, maxValue, wxDefaultPosition, wxSize(Width, wxDefaultCoord), wxSL_AUTOTICKS|wxSL_LABELS|wxSL_TOP);

	m_pCtrl_Sizer->Add(pSlider, 0, wxLEFT|wxRIGHT|wxBOTTOM|wxEXPAND, SGDI_CTRL_SMALLSPACE);

	m_pCtrl->SetVirtualSize(m_pCtrl_Sizer->GetSize());

	return( pSlider );
}

//-----------------------------------------------------------------------------
CSGDI_SpinCtrl * CSGDI_Dialog::Add_SpinCtrl(const wxString &Name, double Value, double minValue, double maxValue, bool bValueAsPercent, int ID, int Width)
{
	Add_Label(Name, true);

	CSGDI_SpinCtrl *pSpinCtrl = new CSGDI_SpinCtrl(m_pCtrl, ID, Value, minValue, maxValue, bValueAsPercent, wxDefaultPosition, wxSize(Width, wxDefaultCoord), wxSP_ARROW_KEYS|wxTE_PROCESS_ENTER);

	m_pCtrl_Sizer->Add(pSpinCtrl, 0, wxALIGN_CENTER|wxLEFT|wxRIGHT|wxBOTTOM|wxEXPAND, SGDI_CTRL_SMALLSPACE);

	m_pCtrl->SetVirtualSize(m_pCtrl_Sizer->GetSize());

	return( pSpinCtrl );
}

//---------------------------------------------------------
void CSGDI_Dialog::Add_CustomCtrl(const wxString &Name, wxWindow *pControl)
{
	Add_Label(Name, true);

	if( pControl->GetParent() != m_pCtrl )
	{
		pControl->Reparent(m_pCtrl);
	}

	m_pCtrl_Sizer->Add(pControl, 0, wxALIGN_CENTER|wxLEFT|wxRIGHT|wxBOTTOM, SGDI_CTRL_SMALLSPACE);

	m_pCtrl->SetVirtualSize(m_pCtrl_Sizer->GetSize());
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
