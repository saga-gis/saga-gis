
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/dcclient.h>

#include "res_images.h"

#include "res/xpm/logo_uhh.xpm"
#include "res/xpm/logo_scilands.xpm"
#include "res/xpm/logo_laserdata.xpm"

#include "saga_gdi/sgdi_helper.h"

#include "dlg_about_logo.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CDLG_About_Logo, wxScrolledCanvas)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CDLG_About_Logo, wxScrolledCanvas)
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
	: wxScrolledCanvas(pParent, -1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER)
{
	SetBackgroundColour(*wxWHITE);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDLG_About_Logo::On_Size(wxSizeEvent &event)
{
	Refresh();
}

//---------------------------------------------------------
void CDLG_About_Logo::On_Paint(wxPaintEvent &event)
{
	wxPaintDC dc(this); DoPrepareDC(dc);

	int Cursor = 0, Space = dc.GetTextExtent("|").y;

	Draw_Text  (dc, Cursor += 1 * Space, "Created and developed by");
	Draw_Text  (dc, Cursor += 0 * Space, "Dr. O. Conrad");

	Draw_Text  (dc, Cursor += 1 * Space, "Core development by");
	Draw_Text  (dc, Cursor += 0 * Space, "Dr. O. Conrad, Dr. V. Wichmann");

	Draw_Text  (dc, Cursor += 0 * Space, "_______________________");

	Draw_Bitmap(dc, Cursor += 0 * Space, logo_uhh_xpm);
	Draw_Text  (dc, Cursor += 0 * Space, "Dr. O. Conrad, Prof. Dr. J. Boehner");

	Draw_Bitmap(dc, Cursor += 3 * Space, logo_scilands_xpm);
	Draw_Text  (dc, Cursor += 1 * Space, "M. Bock, R. Koethe, J. Spitzmueller");

	Draw_Bitmap(dc, Cursor += 3 * Space, logo_laserdata_xpm);
	Draw_Text  (dc, Cursor += 1 * Space, "Dr. V. Wichmann");

	wxBitmap Splash(IMG_Get_Splash(0.3));
	dc.DrawBitmap(Splash, (GetClientSize().x - Splash.GetWidth()) / 2, Cursor += 2 * Space);

	SetVirtualSize(Splash.GetWidth(), Cursor += Splash.GetHeight());
	SetScrollRate(10, 10);
}

//---------------------------------------------------------
bool CDLG_About_Logo::Draw_Text(wxDC &dc, int &Cursor, const wxString &Text)
{
	::Draw_Text(dc, TEXTALIGN_TOPCENTER, GetClientSize().x / 2, Cursor, Text);

	Cursor += dc.GetTextExtent(Text).GetHeight();

	return( true );
}

//---------------------------------------------------------
bool CDLG_About_Logo::Draw_Bitmap(wxDC &dc, int &Cursor, const char *const *XPM)
{
	wxBitmap Bitmap(XPM);

	int x = (GetClientSize().x - Bitmap.GetWidth()) / 2;

	dc.DrawBitmap(Bitmap, x, Cursor, true);

	Cursor += Bitmap.GetHeight();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
