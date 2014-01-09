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
//                     DC_Helper.h                       //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__DC_Helper_H
#define _HEADER_INCLUDED__SAGA_GUI__DC_Helper_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

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
void			Draw_Edge			(wxDC &dc, int Edge_Style, int ax, int ay, int bx, int by);
void			Draw_Edge			(wxDC &dc, int Edge_Style, wxRect r);

//---------------------------------------------------------
void			Draw_Rect			(wxDC &dc, wxColour Color, int ax, int ay, int bx, int by);
void			Draw_Rect			(wxDC &dc, wxColour Color, wxRect r);

//---------------------------------------------------------
void			Draw_FillRect		(wxDC &dc, wxColour Color, int ax, int ay, int bx, int by);
void			Draw_FillRect		(wxDC &dc, wxColour Color, wxRect r);


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
void			Draw_Text			(wxDC &dc, int Align, int x, int y              , const wxString &Text);
void			Draw_Text			(wxDC &dc, int Align, int x, int y, double Angle, const wxString &Text);

void			Draw_Text			(wxDC &dc, int Align, int x, int y              , const wxString &Text, int Effect, wxColour Color);
void			Draw_Text			(wxDC &dc, int Align, int x, int y, double Angle, const wxString &Text, int Effect, wxColour Color);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void			Draw_Scale			(wxDC &dc, wxRect r, double min_Value, double max_Value, bool bHorizontal, bool bAscendent, bool bTickAtTop, bool bLineConnector = false);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__DC_Helper_H
