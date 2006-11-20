
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
	double	d;
	wxCoord	xSize, ySize;

	if( Align != TEXTALIGN_TOPLEFT )
	{
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define TEXTSPACE	4

//---------------------------------------------------------
void		Draw_Scale(wxDC &dc, wxRect r, double zMin, double zMax, bool bHorizontal, bool bAscendent, bool bTickAtTop)
{
	int			xOff, yOff, Width, Height, Height_Tick, Decimals, zDC, yDC, Style, x, y, n;
	double		z, dz, zToDC;
	wxString	s;
	wxFont		Font, oldFont;

	//-----------------------------------------------------
	Style	= bTickAtTop ? 1 : 2;

	Width	= bHorizontal ? r.GetWidth() : r.GetHeight();
	Height	= bHorizontal ? r.GetHeight() : r.GetWidth();

	if( zMin < zMax && Width > 0 && Height > 0 )
	{
		xOff		= r.GetLeft();
		yOff		= r.GetTop();

		oldFont		= dc.GetFont();
		yDC			= (int)((Style == 0 ? 0.60 : 0.45) * (double)Height);
		Font.Create(yDC, wxSWISS, wxNORMAL, wxNORMAL);
		dc.SetFont(Font);

		Height_Tick	= (int)((Style == 0 ? 1.00 : 0.30) * (double)Height);

		//-------------------------------------------------
		zToDC		= (double)Width / (zMax - zMin);

		dz			= pow(10.0, floor(log10(zMax - zMin)) - 1.0);
		Decimals	= dz >= 1.0 ? 0 : (int)fabs(log10(dz));

		s.Printf("%.*f", Decimals, zMax);
		dc.GetTextExtent(s, &zDC, &yDC);
		while( zToDC * dz < zDC + TEXTSPACE )
		{
			dz	*= 2;
		}

		//-------------------------------------------------
		z			= dz * floor(zMin / dz);
		if( Style != 0 && z < zMin )	z	+= dz;

		for(; z<=zMax; z+=dz)
		{
			s.Printf("%.*f", Decimals, z);

			zDC	= bAscendent ? (int)(zToDC * (z - zMin)) : Width - 1 - (int)(zToDC * (z - zMin));

			if( bHorizontal )
			{
				x	= xOff + zDC;
				y	= yOff;
				n	= yOff + Height;

				switch( Style )
				{
				case 0:	// ...
					dc.DrawLine(x, y, x, y + Height_Tick);
					Draw_Text(dc, TEXTALIGN_CENTERLEFT  , x + 2, y + Height / 2, s);
					break;

				case 1:	// tick at top...
					dc.DrawLine(x, y, x, y + Height_Tick);
					Draw_Text(dc, TEXTALIGN_TOPCENTER   , x, y + Height_Tick, s);
					break;

				case 2: // tick at bottom...
					dc.DrawLine(x, n, x, n - Height_Tick);
					Draw_Text(dc, TEXTALIGN_BOTTOMCENTER, x, n - Height_Tick, s);
					break;
				}
			}
			else
			{
				x	= xOff;
				y	= yOff + zDC;
				n	= xOff + Height;

				switch( Style )
				{
				case 0: // ...
					dc.DrawLine(x, y, x + Height_Tick, y);
					Draw_Text(dc, TEXTALIGN_CENTERLEFT  , x + Height / 2, y - 2, 90.0, s);
					break;

				case 1:	// tick at top...
					dc.DrawLine(x, y, x + Height_Tick, y);
					Draw_Text(dc, TEXTALIGN_TOPCENTER   , x + Height_Tick, y, 90.0, s);
					break;

				case 2: // tick at bottom...
					dc.DrawLine(n, y, n - Height_Tick, y);
					Draw_Text(dc, TEXTALIGN_BOTTOMCENTER, n - Height_Tick, y, 90.0, s);
					break;
				}
			}
		}

		//-------------------------------------------------
		dc.SetFont(oldFont);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
