
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
//                    sgdi_helper.h                      //
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
#ifndef HEADER_INCLUDED__SAGA_GDI_sgdi_helper_H
#define HEADER_INCLUDED__SAGA_GDI_sgdi_helper_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "sgdi_core.h"

//---------------------------------------------------------
#include <wx/dc.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	EDGE_STYLE_SIMPLE,
	EDGE_STYLE_STATIC,
	EDGE_STYLE_SUNKEN,
	EDGE_STYLE_RAISED
};

//---------------------------------------------------------
SGDI_API_DLL_EXPORT void	Draw_Edge			(wxDC &dc, int Edge_Style, int ax, int ay, int bx, int by);
SGDI_API_DLL_EXPORT void	Draw_Edge			(wxDC &dc, int Edge_Style, wxRect r);

//---------------------------------------------------------
SGDI_API_DLL_EXPORT void	Draw_Rect			(wxDC &dc, wxColour Color, int ax, int ay, int bx, int by);
SGDI_API_DLL_EXPORT void	Draw_Rect			(wxDC &dc, wxColour Color, wxRect r);

//---------------------------------------------------------
SGDI_API_DLL_EXPORT void	Draw_FillRect		(wxDC &dc, wxColour Color, int ax, int ay, int bx, int by);
SGDI_API_DLL_EXPORT void	Draw_FillRect		(wxDC &dc, wxColour Color, wxRect r);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define TEXTALIGN_LEFT				0x01
#define TEXTALIGN_XCENTER			0x02
#define TEXTALIGN_RIGHT				0x04
#define TEXTALIGN_TOP				0x08
#define TEXTALIGN_YCENTER			0x10
#define TEXTALIGN_BOTTOM			0x20

#define TEXTALIGN_TOPLEFT			(TEXTALIGN_TOP    |TEXTALIGN_LEFT)
#define TEXTALIGN_TOPCENTER			(TEXTALIGN_TOP    |TEXTALIGN_XCENTER)
#define TEXTALIGN_TOPRIGHT			(TEXTALIGN_TOP    |TEXTALIGN_RIGHT)
#define TEXTALIGN_CENTERLEFT		(TEXTALIGN_YCENTER|TEXTALIGN_LEFT)
#define TEXTALIGN_CENTER			(TEXTALIGN_YCENTER|TEXTALIGN_XCENTER)
#define TEXTALIGN_CENTERRIGHT		(TEXTALIGN_YCENTER|TEXTALIGN_RIGHT)
#define TEXTALIGN_BOTTOMLEFT		(TEXTALIGN_BOTTOM |TEXTALIGN_LEFT)
#define TEXTALIGN_BOTTOMCENTER		(TEXTALIGN_BOTTOM |TEXTALIGN_XCENTER)
#define TEXTALIGN_BOTTOMRIGHT		(TEXTALIGN_BOTTOM |TEXTALIGN_RIGHT)

#define	TEXTEFFECT_NONE				0x00
#define	TEXTEFFECT_TOP				0x01
#define	TEXTEFFECT_TOPLEFT			0x02
#define	TEXTEFFECT_LEFT				0x04
#define	TEXTEFFECT_BOTTOMLEFT		0x08
#define	TEXTEFFECT_BOTTOM			0x10
#define	TEXTEFFECT_BOTTOMRIGHT		0x20
#define	TEXTEFFECT_RIGHT			0x40
#define	TEXTEFFECT_TOPRIGHT			0x80
#define	TEXTEFFECT_FRAME			(TEXTEFFECT_TOPLEFT|TEXTEFFECT_BOTTOMLEFT|TEXTEFFECT_BOTTOMRIGHT|TEXTEFFECT_TOPRIGHT)

//---------------------------------------------------------
SGDI_API_DLL_EXPORT void	Draw_Text	(wxDC &dc, int Align, int x, int y, const wxString &Text);
SGDI_API_DLL_EXPORT void	Draw_Text	(wxDC &dc, int Align, int x, int y, double Angle, const wxString &Text);

SGDI_API_DLL_EXPORT void	Draw_Text	(wxDC &dc, int Align, int x, int y              , const wxString &Text, int Effect, wxColour Effect_Color, int Effect_Size = 1);
SGDI_API_DLL_EXPORT void	Draw_Text	(wxDC &dc, int Align, int x, int y, double Angle, const wxString &Text, int Effect, wxColour Effect_Color, int Effect_Size = 1);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SCALE_HORIZONTAL		0x00
#define SCALE_VERTICAL			0x01

//---------------------------------------------------------
#define SCALE_TICK_NONE			0x00
#define SCALE_TICK_TOP			0x01
#define SCALE_TICK_BOTTOM		0x02

//---------------------------------------------------------
#define SCALE_STYLE_DEFAULT		0x00
#define SCALE_STYLE_DESCENDENT	0x02
#define SCALE_STYLE_LINECONN	0x04
#define SCALE_STYLE_BLACKWHITE	0x08
#define SCALE_STYLE_GLOOMING	0x10
#define SCALE_STYLE_UNIT_ABOVE	0x20
#define SCALE_STYLE_UNIT_BELOW	0x40

//---------------------------------------------------------
SGDI_API_DLL_EXPORT void	Draw_Scale	(wxDC &dc, const wxRect &r, double min_Value, double max_Value, int Orientation = SCALE_HORIZONTAL, int Tick = SCALE_TICK_NONE, int Style = SCALE_STYLE_DEFAULT, const wxString &Unit = "");
SGDI_API_DLL_EXPORT void	Draw_Scale	(wxDC &dc, const wxRect &r, double min_Value, double max_Value, bool bHorizontal, bool bAscendent, bool bTickAtTop);

SGDI_API_DLL_EXPORT bool	Draw_Ruler	(wxDC &dc, const wxRect &r, bool bHorizontal, double zMin, double zMax, bool bAscendent = true, int FontSize = 7, const wxColour &Colour = wxColour(127, 127, 127));


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_GDI_sgdi_helper_H
