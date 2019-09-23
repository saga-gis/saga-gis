
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
//                 ACTIVE_Description.cpp                //
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
#include <wx/clipbrd.h>
#include <wx/dataobj.h>

#include "res_controls.h"

#include "helper.h"

#include "active_description.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CActive_Description, wxHtmlWindow)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CActive_Description, wxHtmlWindow)
	EVT_KEY_DOWN		(CActive_Description::On_Key_Down)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CActive_Description::CActive_Description(wxWindow *pParent)
	: wxHtmlWindow(pParent, ID_WND_ACTIVE_DESCRIPTION , wxDefaultPosition, wxDefaultSize, wxHW_SCROLLBAR_AUTO|wxSUNKEN_BORDER)
{
}

//---------------------------------------------------------
CActive_Description::~CActive_Description(void)
{
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CActive_Description::On_Key_Down(wxKeyEvent &event)
{
	if( event.ControlDown() )
	{
		switch( event.GetKeyCode() )
		{
		default:
			event.Skip();
			break;

		case 'C':
		case 'c':
			if( wxTheClipboard->Open() )
			{
				wxTheClipboard->SetData(new wxTextDataObject(SelectionToText()));
				wxTheClipboard->Close();
			}
			break;
		}

	}
}

//---------------------------------------------------------
void CActive_Description::OnLinkClicked(const wxHtmlLinkInfo &Link)
{
	Open_WebBrowser(Link.GetHref());
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
