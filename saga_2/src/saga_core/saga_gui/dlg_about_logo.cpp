
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
#include <wx/image.h>
#include <wx/dcclient.h>

#include "res_images.h"

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
		wxCoord		w, h;
		wxString	s;
		wxPaintDC	dc(this);

		x	= (GetClientSize().x - m_logo.GetWidth()) / 2;
		y	= (GetClientSize().y - (m_logo.GetHeight() + 10 + m_icon.GetHeight())) / 2;
		dc.DrawBitmap(m_logo, x, y, true);

		s.Printf(LNG("System for Automated Geoscientific Analyses"));
		dc.GetTextExtent(s, &w, &h);
		x	= (GetClientSize().x - (m_icon.GetWidth() + 10 + w)) / 2;
		y	+= 10 + m_logo.GetHeight();
		dc.DrawBitmap(m_icon, x, y, true);

		x	+= 10 + m_icon.GetWidth();
		dc.DrawText(s, x, y);
		y	+= 2 + h;
		s.Printf(LNG("Version 2.0, Goettingen, 2006"));
		dc.DrawText(s, x + m_icon.GetWidth() + 10, y);
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
