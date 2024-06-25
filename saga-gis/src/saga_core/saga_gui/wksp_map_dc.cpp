
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
//                   WKSP_Map_DC.cpp                     //
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

#include <saga_gdi/sgdi_helper.h>

#include "helper.h"

#include "wksp_map_dc.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Map_DC::CWKSP_Map_DC(const CSG_Rect &rWorld, const wxRect &rDC, double Scale, int Background, bool bMask)
{
	m_rWorld = rWorld;
	m_rDC    = rDC;
	m_Scale  = Scale;

	m_Background[0]	= Background >= 0 ? SG_GET_R(Background) : 255;
	m_Background[1]	= Background >= 0 ? SG_GET_G(Background) : 255;
	m_Background[2]	= Background >= 0 ? SG_GET_B(Background) : 255;

	//-----------------------------------------------------
	if( m_rWorld.Get_XRange() == 0. || m_rWorld.Get_YRange() == 0. )
	{
		m_rWorld.Inflate(m_rWorld.Get_XRange() ? 0. : 1., m_rWorld.Get_YRange() ? 0. : 1., false);
	}

	// ensure cellsize in x-/y-direction are identical...
	double dxdyDC    = (double)m_rDC.GetWidth() / (double)m_rDC.GetHeight();
	double dxdyWorld = m_rWorld.Get_XRange() / m_rWorld.Get_YRange();

	if( dxdyDC > dxdyWorld )
	{
		m_rWorld.Inflate(0.5 * (m_rWorld.Get_YRange() * dxdyDC - m_rWorld.Get_XRange()), 0., false);
	}
	else if( dxdyDC < dxdyWorld )
	{
		m_rWorld.Inflate(0., 0.5 * (m_rWorld.Get_XRange() / dxdyDC - m_rWorld.Get_YRange()), false);
	}

	//-----------------------------------------------------
	m_World2DC = (double)m_rDC.GetWidth() / m_rWorld.Get_XRange();
	m_DC2World = 1. / m_World2DC;

	//-----------------------------------------------------
	m_bmp.Create(m_rDC.GetWidth(), m_rDC.GetHeight());
	m_dc.SelectObject(m_bmp);
	m_dc.SetBackground(wxBrush(wxColour(m_Background[0], m_Background[1], m_Background[2]), wxBRUSHSTYLE_SOLID));
	m_dc.Clear();

	m_dc_mask.SetBrush(*wxWHITE_BRUSH);
	m_dc_mask.SetPen(*wxWHITE_PEN);
	m_dc_mask.SetTextForeground(*wxWHITE);

	if( bMask )
	{
		m_bmp_mask.Create(m_rDC.GetWidth(), m_rDC.GetHeight());
		m_dc_mask.SelectObject(m_bmp_mask);
		m_dc.SetBackground(*wxBLACK_BRUSH);
	}
}

//---------------------------------------------------------
CWKSP_Map_DC::~CWKSP_Map_DC(void)
{}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Map_DC::Draw(wxDC &dc_Target)
{
	m_dc.SelectObject(wxNullBitmap);

	dc_Target.DrawBitmap(m_bmp, m_rDC.GetLeft(), m_rDC.GetTop(), false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Map_DC::Set_Font(wxFont &Font)
{
	if( Font.Ok() )
	{
		Font.SetPointSize((int)(m_Scale * Font.GetPointSize()));

		SetFont(Font);
	}
}

//---------------------------------------------------------
void CWKSP_Map_DC::SetFont(const wxFont &Font)
{
	m_dc.SetFont(Font);

	if( m_dc_mask.IsOk() )
	{
		m_dc_mask.SetFont(Font);
	}
}

//---------------------------------------------------------
void CWKSP_Map_DC::SetPen(const wxPen &Pen)
{
	m_dc.SetPen(Pen);

	if( m_dc_mask.IsOk() )
	{
		wxPen _Pen(Pen); _Pen.SetColour(*wxWHITE);

		m_dc_mask.SetPen(_Pen);
	}
}

//---------------------------------------------------------
void CWKSP_Map_DC::SetBrush(const wxBrush &Brush)
{
	m_dc.SetBrush(Brush);

	if( m_dc_mask.IsOk() )
	{
		wxBrush _Brush(Brush); _Brush.SetColour(*wxWHITE);

		m_dc_mask.SetBrush(_Brush);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Map_DC::DrawPoint(int x, int y)
{
	m_dc.DrawPoint(x, y);

	if( m_dc_mask.IsOk() )
	{
		m_dc_mask.DrawPoint(x, y);
	}
}

//---------------------------------------------------------
void CWKSP_Map_DC::DrawLine(int x1, int y1, int x2, int y2)
{
	m_dc.DrawLine(x1, y1, x2, y2);

	if( m_dc_mask.IsOk() )
	{
		m_dc_mask.DrawLine(x1, y1, x2, y2);
	}
}

//---------------------------------------------------------
void CWKSP_Map_DC::DrawRectangle(int x, int y, int width, int height)
{
	m_dc.DrawRectangle(x, y, width, height);

	if( m_dc_mask.IsOk() )
	{
		m_dc_mask.DrawRectangle(x, y, width, height);
	}
}

void CWKSP_Map_DC::DrawRectangle(const wxRect &r)
{
	m_dc.DrawRectangle(r);

	if( m_dc_mask.IsOk() )
	{
		m_dc_mask.DrawRectangle(r);
	}
}

//---------------------------------------------------------
void CWKSP_Map_DC::DrawCircle(int x, int y, int radius)
{
	m_dc.DrawCircle(x, y, radius);

	if( m_dc_mask.IsOk() )
	{
		m_dc_mask.DrawCircle(x, y, radius);
	}
}

//---------------------------------------------------------
void CWKSP_Map_DC::DrawArc(int xStart, int yStart, int xEnd, int yEnd, int xc, int yc)
{
	m_dc.DrawArc(xStart, yStart, xEnd, yEnd, xc, yc);

	if( m_dc_mask.IsOk() )
	{
		m_dc_mask.DrawArc(xStart, yStart, xEnd, yEnd, xc, yc);
	}
}

//---------------------------------------------------------
void CWKSP_Map_DC::DrawPolygon(int n, const wxPoint points[], int xoffset, int yoffset, wxPolygonFillMode fill_style)
{
	m_dc.DrawPolygon(n, points, xoffset, yoffset, fill_style);

	if( m_dc_mask.IsOk() )
	{
		m_dc_mask.DrawPolygon(n, points, xoffset, yoffset, fill_style);
	}
}

//---------------------------------------------------------
void CWKSP_Map_DC::DrawPolyPolygon(int n, const int count[], const wxPoint points[], int xoffset, int yoffset, wxPolygonFillMode fill_style)
{
	m_dc.DrawPolyPolygon(n, count, points, xoffset, yoffset, fill_style);

	if( m_dc_mask.IsOk() )
	{
		m_dc_mask.SetPen(*wxWHITE_PEN); m_dc_mask.SetBrush(*wxWHITE_BRUSH);
		m_dc_mask.DrawPolyPolygon(n, count, points, xoffset, yoffset, fill_style);
	}
}

//---------------------------------------------------------
void CWKSP_Map_DC::DrawBitmap(const wxBitmap &bitmap, int x, int y, bool useMask)
{
	m_dc.DrawBitmap(bitmap, x, y, useMask);

	if( m_dc_mask.IsOk() )
	{
		m_dc_mask.DrawBitmap(bitmap, x, y, useMask);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Map_DC::DrawText(int Align, int x, int y              , const wxString &Text)
{
	Draw_Text(m_dc, Align, x, y, Text);

	if( m_dc_mask.IsOk() )
	{
		Draw_Text(m_dc_mask, Align, x, y, Text);
	}
}

//---------------------------------------------------------
void CWKSP_Map_DC::DrawText(int Align, int x, int y, double Angle, const wxString &Text)
{
	Draw_Text(m_dc, Align, x, y, Angle, Text);

	if( m_dc_mask.IsOk() )
	{
		Draw_Text(m_dc_mask, Align, x, y, Angle, Text);
	}
}

//---------------------------------------------------------
void CWKSP_Map_DC::DrawText(int Align, int x, int y              , const wxString &Text, int Effect, wxColour Effect_Color, int Effect_Size)
{
	Draw_Text(m_dc, Align, x, y, Text, Effect, Effect_Color, Effect_Size);

	if( m_dc_mask.IsOk() )
	{
		Draw_Text(m_dc_mask, Align, x, y, Text, Effect, Effect_Color, Effect_Size);
	}
}

//---------------------------------------------------------
void CWKSP_Map_DC::DrawText(int Align, int x, int y, double Angle, const wxString &Text, int Effect, wxColour Effect_Color, int Effect_Size)
{
	Draw_Text(m_dc, Align, x, y, Angle, Text, Effect, Effect_Color, Effect_Size);

	if( m_dc_mask.IsOk() )
	{
		Draw_Text(m_dc_mask, Align, x, y, Angle, Text, Effect, Effect_Color, Effect_Size);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Map_DC::Get_Bitmap(wxBitmap &Bitmap)
{
	if( m_bmp.IsOk() )
	{
		Bitmap = m_bmp;

		if( m_bmp_mask.IsOk() )
		{
			Bitmap.SetMask(new wxMask(m_bmp_mask, *wxBLACK));
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Map_DC::Get_Image(wxImage &Image)
{
	if( m_bmp.IsOk() )
	{
		Image = m_bmp.ConvertToImage();

		if( m_bmp_mask.IsOk() )
		{
			Image.SetAlpha();

			wxImage layer_mask = m_layer_mask.ConvertToImage(); BYTE *mask = layer_mask.GetData();

			#pragma omp parallel for
			for(int y=0; y<m_rDC.GetHeight(); y++)
			{
				for(int x=0, i=y*m_rDC.GetWidth(); x<m_rDC.GetWidth(); x++, i+=3)
				{
					Image.SetAlpha(x, y, mask[i]);
				}
			}
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Map_DC::Draw_Layer_Begin(void)
{
	m_layer_bmp.Create(m_rDC.GetWidth(), m_rDC.GetHeight());
	m_dc.SelectObject(m_layer_bmp);
	m_dc.SetBackground(*wxWHITE_BRUSH); m_dc.Clear();

	m_layer_mask.Create(m_rDC.GetWidth(), m_rDC.GetHeight());
	m_dc_mask.SelectObject(m_layer_mask);
	m_dc_mask.SetBackground(*wxBLACK_BRUSH); m_dc_mask.Clear();

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Map_DC::Draw_Layer_End(double Transparency)
{
	m_dc.SelectObject(m_bmp); m_dc_mask.SelectObject(m_bmp_mask.IsOk() ? m_bmp_mask : wxNullBitmap);

	if( Draw_Image_Begin(Transparency) )
	{
		wxImage layer_rgb  = m_layer_bmp .ConvertToImage(); BYTE *rgb  = layer_rgb .GetData();
		wxImage layer_mask = m_layer_mask.ConvertToImage(); BYTE *mask = layer_mask.GetData();

		#pragma omp parallel for
		for(int i=0; i<m_img_nBytes; i+=3)
		{
			if( mask[i] )
			{
				_Draw_Image_Pixel(i, *((int *)(rgb + i)));
			}
		}

		Draw_Image_End();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CWKSP_Map_DC::Draw_Polygon(CSG_Shape_Polygon *pPolygon)
{
	if( m_World2DC * pPolygon->Get_Extent().Get_XRange() <= 2.
	&&  m_World2DC * pPolygon->Get_Extent().Get_YRange() <= 2. )
	{
		DrawPoint(
			(int)xWorld2DC(pPolygon->Get_Extent().Get_XCenter()),
			(int)yWorld2DC(pPolygon->Get_Extent().Get_YCenter())
		);
	}

	//-----------------------------------------------------
	else if( pPolygon->Get_Part_Count() == 1 && pPolygon->Get_Point_Count(0) > 2 )
	{
		wxPoint	*Points	= new wxPoint[pPolygon->Get_Point_Count()];

		for(int iPoint=0; iPoint<pPolygon->Get_Point_Count(0); iPoint++)
		{
			Points[iPoint].x = (int)xWorld2DC(pPolygon->Get_Point(iPoint, 0).x);
			Points[iPoint].y = (int)yWorld2DC(pPolygon->Get_Point(iPoint, 0).y);
		}

		DrawPolygon(pPolygon->Get_Point_Count(), Points);

		delete[](Points);
	}

	//-----------------------------------------------------
	else if( pPolygon->Get_Part_Count() > 1 )
	{
		wxPoint *Points = new wxPoint[pPolygon->Get_Point_Count()];
		int    *nPoints = new int    [pPolygon->Get_Part_Count ()];

		for(int iPart=0, jPoint=0; iPart<pPolygon->Get_Part_Count(); iPart++)
		{
			nPoints[iPart] = pPolygon->Get_Point_Count(iPart);

			for(int iPoint=0; iPoint<pPolygon->Get_Point_Count(iPart); iPoint++, jPoint++)
			{
				Points[jPoint].x = (int)xWorld2DC(pPolygon->Get_Point(iPoint, iPart).x);
				Points[jPoint].y = (int)yWorld2DC(pPolygon->Get_Point(iPoint, iPart).y);
			}
		}

		DrawPolyPolygon(pPolygon->Get_Part_Count(), nPoints, Points, 0, 0, wxODDEVEN_RULE);

		delete[]( Points);
		delete[](nPoints);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Map_DC::Draw_Image_Begin(double Transparency, int Mode)
{
	if( m_DC2World <= 0. || m_rDC.GetWidth() <= 0 || m_rDC.GetHeight() <= 0 )
	{
		return( false );
	}

	m_Opacity = Transparency < 0. ? 1. : Transparency > 1. ? 0. : 1. - Transparency;

	if( m_Opacity <= 0. && (Mode == IMG_MODE_TRANSPARENT || Mode == IMG_MODE_TRANSPARENT_ALPHA) )
	{
		return( false );
	}

	switch( Mode )
	{
	case IMG_MODE_OPAQUE:   default: m_img_mode = IMG_MODE_OPAQUE                                        ; break;
	case IMG_MODE_SHADING          : m_img_mode = IMG_MODE_SHADING                                       ; break;
	case IMG_MODE_TRANSPARENT      : m_img_mode = m_Opacity < 1. ? IMG_MODE_TRANSPARENT : IMG_MODE_OPAQUE; break;
	case IMG_MODE_TRANSPARENT_ALPHA: m_img_mode = IMG_MODE_TRANSPARENT_ALPHA                             ; break;
	}

	//-----------------------------------------------------
	wxMemoryDC copy_dc; wxBitmap copy_bmp(m_rDC.GetWidth(), m_rDC.GetHeight());

	copy_dc.SelectObject(copy_bmp);
	copy_dc.Blit(0, 0, m_rDC.GetWidth(), m_rDC.GetHeight(), &m_dc, 0, 0);
	copy_dc.SelectObject(wxNullBitmap);

	if( copy_bmp.IsOk() )
	{
		m_img = copy_bmp.ConvertToImage();

		if( m_img.IsOk() )
		{
			m_img_rgb = m_img.GetData(); m_img_nx = m_rDC.GetWidth(); m_img_nBytes = 3 * m_rDC.GetHeight() * m_rDC.GetWidth();

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Map_DC::Draw_Image_End(void)
{
	if( m_img.Ok() )
	{
		m_dc.DrawBitmap(wxBitmap(m_img), 0, 0, true);

		m_img.Destroy(); m_img_rgb = NULL; m_img_nx = 0; m_img_nBytes = 0;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CWKSP_Map_DC::Draw_Image_Pixels(int x1, int y1, int x2, int y2, int Color)
{
	if( y1 >  y2                ) { double y = y1; y1 = y2; y2 = y; }
	if( y1 <  0                 ) { y1 = 0; }
	if( y2 >= m_rDC.GetHeight() ) { y2 = m_rDC.GetHeight(); }

	if( x1 > x2                 ) { double x = x1; x1 = x2; x2 = x; }
	if( x1 < 0                  ) { x1 = 0; }
	if( x2 >= m_rDC.GetWidth () ) { x2 = m_rDC.GetWidth (); }

	//-----------------------------------------------------
	for(int y=y1; y<y2; y++)
	{
		int i = 3 * (y * m_img_nx + x1);

		for(int x=x1; x<x2; x++, i+=3)
		{
			_Draw_Image_Pixel(i, Color);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
