
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
//                    DLG_Base.cpp                       //
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
#include <wx/window.h>
#include <wx/panel.h>
#include <wx/button.h>

#include "res_controls.h"
#include "res_dialogs.h"
#include "res_images.h"

#include "helper.h"

#include "dlg_base.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CDLG_Base, wxDialog)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CDLG_Base, wxDialog)
	EVT_SIZE			(CDLG_Base::On_Size)
	EVT_KEY_DOWN		(CDLG_Base::On_Key )

	EVT_BUTTON			(wxID_OK    , CDLG_Base::On_Ok    )
	EVT_BUTTON			(wxID_CANCEL, CDLG_Base::On_Cancel)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDLG_Base::CDLG_Base(int id, wxString Caption, bool bCancelBtn)
	: wxDialog(MDI_Get_Frame(), id, Caption, DLG_Get_Def_Position(), DLG_Get_Def_Size(), wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
{
#ifndef WITH_WXBMPBUNDLE
	SetIcon (IMG_Get_Icon (ID_IMG_SAGA_ICON_32));
#else
	SetIcons(IMG_Get_Icons(ID_IMG_SAGA_ICON));
#endif

	MDI_Top_Window_Push(this);

	m_pPanel_Controls = new wxPanel(this);

	m_nControls = 0;

	Add_Button(wxID_OK);

	if( bCancelBtn )
	{
		Add_Button(wxID_CANCEL);
	}

	m_nControls++;
}

//---------------------------------------------------------
CDLG_Base::~CDLG_Base(void)
{
	MDI_Top_Window_Pop(this);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Base::On_Size(wxSizeEvent &event)
{
	bool	bResize	= false;
	wxSize	Size	= event.GetSize();	//	s	= GetClientSize();

	if( Size.GetWidth() < 300 )
	{
		Size.SetWidth(300);
		bResize	= true;
	}

	if( Size.GetHeight() < 300 )
	{
		Size.SetHeight(300);
		bResize	= true;
	}

	if( bResize )
	{
		SetSize(Size);
	}

	Set_Positions();

	event.Skip();
}

//---------------------------------------------------------
void CDLG_Base::On_Key(wxKeyEvent &event)
{
	if( event.GetKeyCode() == WXK_RETURN )
	{
		_Exit(true);
	}
	else
	{
		event.Skip();
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define CONTROL_HEIGHT	wxButton::GetDefaultSize().y
#define CONTROL_WIDTH	70
#define CONTROL_DIST	5

//---------------------------------------------------------
void CDLG_Base::Add_Control(wxControl *pControl)
{
	if( pControl )
	{
		pControl->SetSize(0, CONTROL_DIST + m_nControls * (CONTROL_DIST + CONTROL_HEIGHT), CONTROL_WIDTH, CONTROL_HEIGHT);
	}

	m_nControls++;
}

//---------------------------------------------------------
wxButton * CDLG_Base::Add_Button(int Button_ID)
{
	wxButton	*pButton	= Button_ID > 0 ? new wxButton(m_pPanel_Controls, Button_ID, CTRL_Get_Name(Button_ID)) : NULL;

	Add_Control(pButton);

	return( pButton );
}

//---------------------------------------------------------
void CDLG_Base::Set_Positions(void)
{
	int		xDivide, yTotal;

	xDivide	= GetClientSize().GetWidth() - CONTROL_WIDTH - 2 * CONTROL_DIST;
	yTotal	= GetClientSize().GetHeight() - 2 * CONTROL_DIST;

	m_pPanel_Controls->SetSize(wxRect(xDivide + CONTROL_DIST, CONTROL_DIST, CONTROL_WIDTH, yTotal));

	Set_Position(wxRect(CONTROL_DIST, CONTROL_DIST, xDivide - 2 * CONTROL_DIST, yTotal));
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_Base::On_Ok(wxCommandEvent &event)
{
	_Exit(true);
}

//---------------------------------------------------------
void CDLG_Base::On_Cancel(wxCommandEvent &event)
{
	_Exit(false);
}

//---------------------------------------------------------
void CDLG_Base::_Exit(bool bOk)
{
	if( bOk )
	{
		Save_Changes();

		EndModal(wxID_OK);
	}
	else
	{
		EndModal(wxID_CANCEL);
	}
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
