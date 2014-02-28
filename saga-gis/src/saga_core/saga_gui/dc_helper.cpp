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
//                    DC_Helper.cpp                      //
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
#include <wx/window.h>

#include <saga_api/saga_api.h>

#include "helper.h"
#include "dc_helper.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void		Draw_Edge(wxDC &dc, int Edge_Style, int ax, int ay, int bx, int by)
{
	wxPen	oldPen;

	switch( Edge_Style )
	{
	//-----------------------------------------------------
	case EDGE_STYLE_SIMPLE:
		dc.DrawLine(bx, ay, bx, by);
		dc.DrawLine(bx, by, ax, by);
		dc.DrawLine(ax, ay, bx, ay);
		dc.DrawLine(ax, by, ax, ay);
		break;

	//-----------------------------------------------------
	case EDGE_STYLE_STATIC:
		Draw_Edge(dc, EDGE_STYLE_SUNKEN, ax + 0, ay + 0, bx - 0, by - 0);
		Draw_Edge(dc, EDGE_STYLE_RAISED, ax + 1, ay + 1, bx - 1, by - 1);
		break;

	//-----------------------------------------------------
	case EDGE_STYLE_SUNKEN:
		oldPen	= dc.GetPen();

		dc.SetPen(wxPen(SYS_Get_Color(wxSYS_COLOUR_BTNHIGHLIGHT), 0, wxSOLID));
		dc.DrawLine(bx, ay, bx, by);
		dc.DrawLine(bx, by, ax, by);

		dc.SetPen(wxPen(SYS_Get_Color(wxSYS_COLOUR_BTNSHADOW   ), 0, wxSOLID));
		dc.DrawLine(ax, by, ax, ay);
		dc.DrawLine(ax, ay, bx, ay);

		dc.SetPen(oldPen);
		break;

	//-----------------------------------------------------
	case EDGE_STYLE_RAISED:
		oldPen	= dc.GetPen();

		dc.SetPen(wxPen(SYS_Get_Color(wxSYS_COLOUR_BTNSHADOW   ), 0, wxSOLID));
		dc.DrawLine(bx, ay, bx, by);
		dc.DrawLine(bx, by, ax, by);

		dc.SetPen(wxPen(SYS_Get_Color(wxSYS_COLOUR_BTNHIGHLIGHT), 0, wxSOLID));
		dc.DrawLine(ax, by, ax, ay);
		dc.DrawLine(ax, ay, bx, ay);

		dc.SetPen(oldPen);
		break;
	}
}

//---------------------------------------------------------
void		Draw_Edge(wxDC &dc, int Edge_Style, wxRect r)
{
	Draw_Edge(dc, Edge_Style, r.GetLeft(), r.GetTop(), r.GetRight(), r.GetBottom());
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void		Draw_Rect(wxDC &dc, wxColour Color, int ax, int ay, int bx, int by)
{
	Draw_FillRect(dc, Color, ax, ay, bx, by);
	Draw_Edge(dc, EDGE_STYLE_SIMPLE, ax, ay, bx, by);
}

//---------------------------------------------------------
void		Draw_Rect(wxDC &dc, wxColour Color, wxRect r)
{
	Draw_Rect(dc, Color, r.GetLeft(), r.GetTop(), r.GetRight(), r.GetBottom());
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void		Draw_FillRect(wxDC &dc, wxColour Color, int ax, int ay, int bx, int by)
{
	int		i;
	wxPen	Pen, oldPen;

	//-----------------------------------------------------
	oldPen	= dc.GetPen();
	Pen.SetColour(Color);
	dc.SetPen(Pen);

	if( ax > bx )
	{
		i	= ax;	ax	= bx;	bx	= i;
	}

	if( ay > by )
	{
		i	= ay;	ay	= by;	by	= i;
	}

	//-----------------------------------------------------
	if( bx - ax < by - ay )
	{
		for(i=ax; i<bx; i++)
		{
			dc.DrawLine(i, ay, i, by);
		}
	}
	else
	{
		for(i=ay; i<by; i++)
		{
			dc.DrawLine(ax, i, bx, i);
		}
	}

	//-----------------------------------------------------
	dc.SetPen(oldPen);
}

//---------------------------------------------------------
void		Draw_FillRect(wxDC &dc, wxColour Color, wxRect r)
{
	Draw_FillRect(dc, Color, r.GetLeft(), r.GetTop(), r.GetRight(), r.GetBottom());
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void		Draw_Text(wxDC &dc, int Align, int x, int y, const wxString &Text)
{
	wxCoord	xSize, ySize;

	if( Align != TEXTALIGN_TOPLEFT )
	{
		dc.GetTextExtent(Text, &xSize, &ySize);

		//-------------------------------------------------
		if		( Align & TEXTALIGN_XCENTER )
		{
			x	-= xSize / 2;
		}
		else if	( Align & TEXTALIGN_RIGHT )
		{
			x	-= xSize;
		}

		//-------------------------------------------------
		if		( Align & TEXTALIGN_YCENTER )
		{
			y	-= ySize / 2;
		}
		else if	( Align & TEXTALIGN_BOTTOM )
		{
			y	-= ySize;
		}
	}

	dc.DrawText(Text, x, y);
}

//---------------------------------------------------------
void		Draw_Text(wxDC &dc, int Align, int x, int y, double Angle, const wxString &Text)
{
	if( Angle == 0.0 )
	{
		Draw_Text(dc, Align, x, y, Text);

		return;
	}

	wxCoord	xSize, ySize;

	if( Align != TEXTALIGN_TOPLEFT )
	{
		double	d;

		dc.GetTextExtent(Text, &xSize, &ySize);

		//-------------------------------------------------
		d	 = M_DEG_TO_RAD * Angle;

		if		( Align & TEXTALIGN_XCENTER )
		{
			x	-= (int)(xSize * cos(d) / 2.0);
			y	+= (int)(xSize * sin(d) / 2.0);
		}
		else if	( Align & TEXTALIGN_RIGHT )
		{
			x	-= (int)(xSize * cos(d));
			y	+= (int)(xSize * sin(d));
		}

		//-------------------------------------------------
		d	 = M_DEG_TO_RAD * (Angle - 90.0);

		if		( Align & TEXTALIGN_YCENTER )
		{
			x	-= (int)(ySize * cos(d) / 2.0);
			y	+= (int)(ySize * sin(d) / 2.0);
		}
		else if	( Align & TEXTALIGN_BOTTOM )
		{
			x	-= (int)(ySize * cos(d));
			y	+= (int)(ySize * sin(d));
		}
	}

	dc.DrawRotatedText(Text, x, y, Angle);
}

//---------------------------------------------------------
void			Draw_Text			(wxDC &dc, int Align, int x, int y, const wxString &Text, int Effect, wxColour Color)
{
	Draw_Text(dc, Align, x, y, 0.0, Text, Effect, Color);
}

void			Draw_Text			(wxDC &dc, int Align, int x, int y, double Angle, const wxString &Text, int Effect, wxColour Color)
{
	if( Effect != TEXTEFFECT_NONE )
	{
		int			d			= 1;
		wxColour	oldColor	= dc.GetTextForeground();	dc.SetTextForeground(Color);

		if( Effect & TEXTEFFECT_TOP         )	Draw_Text(dc, Align, x    , y - d, Angle, Text);
		if( Effect & TEXTEFFECT_TOPLEFT     )	Draw_Text(dc, Align, x - d, y - d, Angle, Text);
		if( Effect & TEXTEFFECT_LEFT        )	Draw_Text(dc, Align, x - d, y    , Angle, Text);
		if( Effect & TEXTEFFECT_BOTTOMLEFT  )	Draw_Text(dc, Align, x - d, y + d, Angle, Text);
		if( Effect & TEXTEFFECT_BOTTOM      )	Draw_Text(dc, Align, x    , y + d, Angle, Text);
		if( Effect & TEXTEFFECT_BOTTOMRIGHT )	Draw_Text(dc, Align, x + d, y + d, Angle, Text);
		if( Effect & TEXTEFFECT_RIGHT       )	Draw_Text(dc, Align, x + d, y    , Angle, Text);
		if( Effect & TEXTEFFECT_TOPRIGHT    )	Draw_Text(dc, Align, x + d, y - d, Angle, Text);

		dc.SetTextForeground(oldColor);
	}

	Draw_Text(dc, Align, x, y, Angle, Text);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define TEXTSPACE	6

//---------------------------------------------------------
void		Draw_Scale(wxDC &dc, const wxRect &r, double zMin, double zMax, int Orientation, int Tick, int Style, const wxString &Unit)
{
	//-----------------------------------------------------
	int	Width	= Orientation != SCALE_VERTICAL ? r.GetWidth() : r.GetHeight();
	int	Height	= Orientation != SCALE_VERTICAL ? r.GetHeight() : r.GetWidth();

	if( zMin >= zMax || Width < 5 || Height < 5 )
	{
		return;
	}

	//-----------------------------------------------------
	if( Style & SCALE_STYLE_GLOOMING )
	{
		Style	^= SCALE_STYLE_GLOOMING;

		wxRect	rTmp;

		dc.SetPen     (wxPen(*wxWHITE));
		dc.SetTextForeground(*wxWHITE);

		rTmp	= r; rTmp.Offset( 0,  1); Draw_Scale(dc, rTmp, zMin, zMax, Orientation, Tick, Style);
		rTmp	= r; rTmp.Offset( 1,  1); Draw_Scale(dc, rTmp, zMin, zMax, Orientation, Tick, Style);
		rTmp	= r; rTmp.Offset( 1,  0); Draw_Scale(dc, rTmp, zMin, zMax, Orientation, Tick, Style);
		rTmp	= r; rTmp.Offset( 1, -1); Draw_Scale(dc, rTmp, zMin, zMax, Orientation, Tick, Style);
		rTmp	= r; rTmp.Offset( 0, -1); Draw_Scale(dc, rTmp, zMin, zMax, Orientation, Tick, Style);
		rTmp	= r; rTmp.Offset(-1, -1); Draw_Scale(dc, rTmp, zMin, zMax, Orientation, Tick, Style);
		rTmp	= r; rTmp.Offset(-1,  0); Draw_Scale(dc, rTmp, zMin, zMax, Orientation, Tick, Style);
		rTmp	= r; rTmp.Offset(-1,  1); Draw_Scale(dc, rTmp, zMin, zMax, Orientation, Tick, Style);

		dc.SetPen     (wxPen(*wxBLACK));
		dc.SetTextForeground(*wxBLACK);
	}

	//-----------------------------------------------------
	int			xOff, yOff, Height_Tick, Decimals, zDC, yDC, x, y, n;
	double		z, dz, zToDC;
	wxString	s;
	wxFont		Font, oldFont;

	//-----------------------------------------------------
	xOff		= r.GetLeft();
	yOff		= r.GetTop();

	oldFont		= dc.GetFont();
	yDC			= (int)((Tick == SCALE_TICK_NONE ? 0.60 : 0.45) * (double)Height);
	Font.Create(yDC, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	dc.SetFont(Font);

	Height_Tick	= (int)((Tick == SCALE_TICK_NONE ? 1.00 : 0.30) * (double)Height);

	//-----------------------------------------------------
	zToDC		= (double)Width / (zMax - zMin);

	dz			= pow(10.0, floor(log10(zMax - zMin)) - 1.0);
	Decimals	= dz >= 1.0 ? 0 : (int)floor(-log10(dz));

	s.Printf(wxT("%.*f"), Decimals, zMax);
	dc.GetTextExtent(s, &zDC, &yDC);
	while( zToDC * dz < zDC + TEXTSPACE )
	{
		dz	*= 2;
	}

	//-----------------------------------------------------
	z			= dz * floor(zMin / dz);

	if( Tick != SCALE_TICK_NONE && z < zMin )
	{
		z	+= dz;
	}

	if( z == z + dz )
	{
		return;
	}

	//-----------------------------------------------------
	for(; z<=zMax; z+=dz)
	{
		s.Printf(wxT("%.*f"), Decimals, z);

		zDC	= !(Style & SCALE_STYLE_DESCENDENT) ? (int)(zToDC * (z - zMin)) : Width - 1 - (int)(zToDC * (z - zMin));

		if( Orientation != SCALE_VERTICAL )
		{
			x	= xOff + zDC;
			y	= yOff;
			n	= yOff + Height;

			switch( Tick )
			{
			default:
				dc.DrawLine(x, y, x, y + Height_Tick);
				Draw_Text(dc, TEXTALIGN_CENTERLEFT  , x + 2, y + Height / 2, s);
				break;

			case SCALE_TICK_TOP:
				dc.DrawLine(x, y, x, y + Height_Tick);
				Draw_Text(dc, TEXTALIGN_TOPCENTER   , x, y + Height_Tick, s);
				break;

			case SCALE_TICK_BOTTOM:
				dc.DrawLine(x, n, x, n - Height_Tick);
				Draw_Text(dc, TEXTALIGN_BOTTOMCENTER, x, n - Height_Tick, s);
				break;
			}
		}

		else // if( Orientation == SCALE_VERTICAL )
		{
			x	= xOff;
			y	= yOff + zDC;
			n	= xOff + Height;

			switch( Tick )
			{
			default:
				dc.DrawLine(x, y, x + Height_Tick, y);
				Draw_Text(dc, TEXTALIGN_CENTERLEFT  , x + Height / 2, y - 2, 90.0, s);
				break;

			case SCALE_TICK_TOP:
				dc.DrawLine(x, y, x + Height_Tick, y);
				Draw_Text(dc, TEXTALIGN_TOPCENTER   , x + Height_Tick, y, 90.0, s);
				break;

			case SCALE_TICK_BOTTOM:
				dc.DrawLine(n, y, n - Height_Tick, y);
				Draw_Text(dc, TEXTALIGN_BOTTOMCENTER, n - Height_Tick, y, 90.0, s);
				break;
			}
		}
	}

	//-----------------------------------------------------
	if( Style & SCALE_STYLE_LINECONN )
	{
		if( Orientation != SCALE_VERTICAL )
		{
			dc.DrawLine(xOff, yOff, xOff + zDC, yOff);
		}
		else
		{
		}
	}

	//-----------------------------------------------------
	dc.SetFont(oldFont);
}

//---------------------------------------------------------
void		Draw_Scale(wxDC &dc, const wxRect &r, double zMin, double zMax, bool bHorizontal, bool bAscendent, bool bTickAtTop)
{
	Draw_Scale(dc, r, zMin, zMax, bHorizontal ? SCALE_HORIZONTAL : SCALE_VERTICAL, bTickAtTop ? SCALE_TICK_TOP : SCALE_TICK_BOTTOM, bAscendent ? SCALE_STYLE_DEFAULT : SCALE_STYLE_DESCENDENT);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
