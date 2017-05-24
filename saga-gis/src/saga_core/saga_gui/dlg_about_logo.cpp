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
//                  DLG_About_Logo.cpp                   //
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
#include <wx/image.h>
#include <wx/dcclient.h>

#include "res_images.h"

#include "helper.h"

#include "dlg_about_logo.h"

#include <saga_api/api_core.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CDLG_About_Logo, wxPanel)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CDLG_About_Logo, wxPanel)
	EVT_SIZE			(CDLG_About_Logo::On_Size)
	EVT_PAINT			(CDLG_About_Logo::On_Paint)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDLG_About_Logo::CDLG_About_Logo(wxWindow *pParent)
	: wxPanel(pParent, -1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER)
{
	SYS_Set_Color_BG_Window(this);

	m_logo	= IMG_Get_Bitmap(ID_IMG_SAGA_SPLASH);
	m_icon	= IMG_Get_Bitmap(ID_IMG_SAGA_ICON_32);
}

//---------------------------------------------------------
CDLG_About_Logo::~CDLG_About_Logo(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_About_Logo::On_Paint(wxPaintEvent &event)
{
	if( m_logo.Ok() && m_icon.Ok() )
	{
		int			x, y;
		wxString	s;
		wxPaintDC	dc(this);

		x	= (GetClientSize().x - m_logo.GetWidth()) / 2;
		y	= (GetClientSize().y - (m_logo.GetHeight() + 10 + m_icon.GetHeight())) / 2;
		dc.DrawBitmap(m_logo, x, y, true);
	}
}

//---------------------------------------------------------
void CDLG_About_Logo::On_Size(wxSizeEvent &event)
{
	Refresh();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
