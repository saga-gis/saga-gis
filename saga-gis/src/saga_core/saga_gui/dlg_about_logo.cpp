
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
#ifdef wxHAS_SVG
#include "res/svg/logo_laserdata.svg.h"
#else
#include "res/xpm/logo_laserdata.xpm"
#endif

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
	: wxScrolledCanvas(pParent, -1, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE)
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
	#define FONT_ITALIC dc.SetFont(Font.Italic());
	#define FONT_NORMAL dc.SetFont(Font);

	wxPaintDC dc(this); DoPrepareDC(dc); wxFont Font(dc.GetFont());

	int Cursor = 0, Space = dc.GetTextExtent("|").y;

	FONT_NORMAL; Draw_Text(dc, Cursor += Space * 1, "Created and developed by");
	FONT_ITALIC; Draw_Text(dc, Cursor += Space * 0, "Dr. O. Conrad");

	FONT_NORMAL; Draw_Text(dc, Cursor += Space * 1, "Core Team and Administration");
	FONT_ITALIC; Draw_Text(dc, Cursor += Space * 0, "Dr. O. Conrad, Dr. V. Wichmann");

	FONT_NORMAL; Draw_Text(dc, Cursor += Space * 0, "_______________________");

	Draw_XPM              (dc, Cursor += Space * 0, xpm_logo_uhh);
	FONT_NORMAL; Draw_Text(dc, Cursor += Space * 0, "Department of Physical Geography");
	FONT_ITALIC; Draw_Text(dc, Cursor += Space * 0, "Prof. Dr. J. Boehner, Dr. O. Conrad");

	Draw_XPM              (dc, Cursor += Space * 3, xpm_logo_scilands);
	FONT_ITALIC; Draw_Text(dc, Cursor += Space / 2, "M. Bock, R. Koethe, J. Spitzmueller");

#ifdef wxHAS_SVG
	Draw_SVG              (dc, Cursor += Space * 3, svg_logo_laserdata, wxSize(213, 47)); // size corresponds to the uhh-logo width
#else
	Draw_XPM              (dc, Cursor += Space * 3, xpm_logo_laserdata);
#endif
	FONT_ITALIC; Draw_Text(dc, Cursor += Space / 2, "Dr. V. Wichmann");

	FONT_NORMAL; Draw_Text(dc, Cursor += Space / 2, "_______________________"); Cursor += Space;
	Draw_Text(dc, Cursor, "We also like to thank you for all kind of your contributions");
	Draw_Text(dc, Cursor, "like package management, bug fix reports, feature");
	Draw_Text(dc, Cursor, "suggestions, forum questions and answers, documentation");
	Draw_Text(dc, Cursor, "and tutorials, citations and recommendations,");
	Draw_Text(dc, Cursor, "...and for choosing SAGA!");

	wxBitmap Splash(IMG_Get_Splash(0.3));
	dc.DrawBitmap(Splash, (GetClientSize().x - Splash.GetWidth()) / 2, Cursor += Space);

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
bool CDLG_About_Logo::Draw_XPM(wxDC &dc, int &Cursor, const char *const *XPM)
{
	wxBitmap Bitmap(XPM);

	int x = (GetClientSize().x - Bitmap.GetWidth()) / 2;

	dc.DrawBitmap(Bitmap, x, Cursor, true);

	Cursor += Bitmap.GetHeight();

	return( true );
}

//---------------------------------------------------------
#ifdef wxHAS_SVG
bool CDLG_About_Logo::Draw_SVG(wxDC &dc, int &Cursor, const char *SVG, const wxSize &Size)
{
	wxBitmap Bitmap(wxBitmapBundle::FromSVG(SVG, Size).GetBitmap(Size));

	int x = (GetClientSize().x - Bitmap.GetWidth()) / 2;

	dc.DrawBitmap(Bitmap, x, Cursor, true);

	Cursor += Bitmap.GetHeight();

	return( true );
}
#endif


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
