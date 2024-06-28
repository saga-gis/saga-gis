
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
//                      map_dc.cpp                       //
//                                                       //
//                 Copyright (C) 2024 by                 //
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
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "sgdi_helper.h"

#include "map_dc.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Map_DC::CSG_Map_DC(void)
{
	// nop
}

//---------------------------------------------------------
CSG_Map_DC::CSG_Map_DC(const CSG_Rect &rWorld, const wxRect &rDC, double Scale, int Background, bool bMask)
{
	Create(rWorld, rDC, Scale, Background, bMask);
}

//---------------------------------------------------------
CSG_Map_DC::~CSG_Map_DC(void)
{
	Destroy();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Map_DC::Create(const CSG_Rect &rWorld, const wxRect &rDC, double Scale, int Background, bool bMask)
{
	m_rWorld = rWorld;
	m_rDC    = rDC;
	m_Scale  = Scale;

	m_img_nx = m_rDC.GetWidth ();
	m_img_ny = m_rDC.GetHeight(); m_img_nPixels = m_img_nx * m_img_ny;

	if( m_img_nx < 1 || m_img_ny < 1 )
	{
		Destroy();

		return( false );
	}

	//-----------------------------------------------------
	if( m_rWorld.Get_XRange() == 0. || m_rWorld.Get_YRange() == 0. )
	{
		m_rWorld.Inflate(m_rWorld.Get_XRange() ? 0. : 1., m_rWorld.Get_YRange() ? 0. : 1., false);
	}

	// ensure cellsize in x-/y-direction are identical...
	double dxdyDC    = (double)m_img_nx / (double)m_img_ny;
	double dxdyWorld = m_rWorld.Get_XRange() / m_rWorld.Get_YRange();

	if( dxdyDC > dxdyWorld )
	{
		m_rWorld.Inflate(0.5 * (m_rWorld.Get_YRange() * dxdyDC - m_rWorld.Get_XRange()), 0., false);
	}
	else if( dxdyDC < dxdyWorld )
	{
		m_rWorld.Inflate(0., 0.5 * (m_rWorld.Get_XRange() / dxdyDC - m_rWorld.Get_YRange()), false);
	}

	m_World2DC = m_img_nx / m_rWorld.Get_XRange();
	m_DC2World = m_rWorld.Get_XRange() / m_img_nx;

	//-----------------------------------------------------
	m_bmp.Create(m_img_nx, m_img_ny);
	m_dc.SelectObject(m_bmp);
	m_dc.SetBackground(wxBrush(Background < 0 ? *wxWHITE : wxColour(SG_GET_R(Background), SG_GET_G(Background), SG_GET_B(Background))));
	m_dc.Clear();

	if( (m_img_mask = bMask ? (BYTE *)m_Mask.Create(sizeof(BYTE), m_img_nPixels) : NULL) != NULL )
	{
		memset(m_img_mask, 0, m_img_nPixels);
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Map_DC::Destroy(void)
{
	m_img.Destroy(); m_img_rgb = NULL; m_img_mask = NULL; m_img_nx = 0; m_img_nPixels = 0;

	m_Mask.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxBitmap CSG_Map_DC::Get_Bitmap(void)
{
	wxBitmap Bitmap; Get_Bitmap(Bitmap); return( Bitmap );
}

//---------------------------------------------------------
bool CSG_Map_DC::Get_Bitmap(wxBitmap &Bitmap)
{
	if( m_bmp.IsOk() )
	{
		if( m_img_mask )
		{
			wxImage Image;

			if( Get_Image(Image) )
			{
				Bitmap = wxBitmap(Image);

				return( true );
			}
		}
		else
		{
			m_dc.SelectObject(wxNullBitmap);

			Bitmap = m_bmp;

			m_dc.SelectObject(m_bmp);

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Map_DC::Get_Image(wxImage &Image)
{
	if( m_bmp.IsOk() )
	{
		m_dc.SelectObject(wxNullBitmap);

		Image = m_bmp.ConvertToImage();

		if( m_img_mask )
		{
			if( Image.HasAlpha() == false )
			{
				Image.SetAlpha();
			}

			#pragma omp parallel for
			for(int y=0; y<m_img_ny; y++)
			{
				for(int x=0, i=y*m_img_nx; x<m_img_nx; x++, i++)
				{
					Image.SetAlpha(x, y, m_img_mask[i]);
				}
			}
		}

		m_dc.SelectObject(m_bmp);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Map_DC::Draw_Layer_Begin(void)
{
	m_bmp_layer.Create(m_img_nx, m_img_ny);
	m_dc       .SelectObject(m_bmp_layer);
	m_dc       .SetBackground    (*wxWHITE_BRUSH);
	m_dc       .Clear();

	m_bmp_mask .Create(m_img_nx, m_img_ny);
	m_dc_mask  .SelectObject(m_bmp_mask);
	m_dc_mask  .SetBrush         (*wxWHITE_BRUSH);
	m_dc_mask  .SetPen           (*wxWHITE_PEN);
	m_dc_mask  .SetTextForeground(*wxWHITE);
	m_dc_mask  .SetBackground    (*wxBLACK_BRUSH);
	m_dc_mask  .Clear();

	return( true );
}

//---------------------------------------------------------
bool CSG_Map_DC::Draw_Layer_End(double Transparency)
{
	m_dc.SelectObject(m_bmp); m_dc_mask.SelectObject(wxNullBitmap);

	if( Draw_Image_Begin(Transparency) )
	{
		wxImage img_layer = m_bmp_layer.ConvertToImage(); BYTE *rgb  = img_layer.GetData();
		wxImage img_mask  = m_bmp_mask .ConvertToImage(); BYTE *mask = img_mask .GetData();

		#pragma omp parallel for
		for(int i=0; i<m_img_nPixels; i++)
		{
			if( mask[i * 3] )
			{
				_Draw_Image_Pixel(i, *((int *)(rgb + i * 3)));
			}
		}

		Draw_Image_End();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CSG_Map_DC::Draw_Polygon(CSG_Shape_Polygon *pPolygon)
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
bool CSG_Map_DC::Draw_Image_Begin(double Transparency, Mode Mode)
{
	if( m_DC2World <= 0. || m_img_nx < 1 || m_img_ny < 1 )
	{
		return( false );
	}

	m_Opacity = Transparency < 0. ? 1. : Transparency > 1. ? 0. : 1. - Transparency;

	if( m_Opacity <= 0. && (Mode == Mode::Transparent || Mode == Mode::Alpha) )
	{
		return( false );
	}

	if( Mode == Mode::Transparent && m_Opacity >= 1. )
	{
		Mode = Mode::Opaque;
	}

	m_img_mode = Mode;

	//-----------------------------------------------------
	wxMemoryDC copy_dc; wxBitmap copy_bmp(m_img_nx, m_img_ny);

	copy_dc.SelectObject(copy_bmp);
	copy_dc.Blit(0, 0, m_img_nx, m_img_ny, &m_dc, 0, 0);
	copy_dc.SelectObject(wxNullBitmap);

	if( copy_bmp.IsOk() )
	{
		m_img = copy_bmp.ConvertToImage();

		if( m_img.IsOk() )
		{
			m_img_rgb = m_img.GetData();

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Map_DC::Draw_Image_End(void)
{
	if( m_img.Ok() )
	{
		m_dc.DrawBitmap(wxBitmap(m_img), 0, 0, true);

		m_img.Destroy(); m_img_rgb = NULL;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CSG_Map_DC::Draw_Image_Pixels(int x1, int y1, int x2, int y2, int Color)
{
	if( x1 > x2 ) { double x = x1; x1 = x2; x2 = x; }
	if( y1 > y2 ) { double y = y1; y1 = y2; y2 = y; }

	if( y1 < 0        ) { y1 = 0;        }
	if( y2 > m_img_ny ) { y2 = m_img_ny; }

	if( x1 < 0        ) { x1 = 0;        }
	if( x2 > m_img_nx ) { x2 = m_img_nx; }

	//-----------------------------------------------------
	for(int y=y1; y<y2; y++)
	{
		for(int x=x1, i=y*m_img_nx+x1; x<x2; x++, i++)
		{
			_Draw_Image_Pixel(i, Color);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Map_DC::Set_Font(wxFont &Font)
{
	if( Font.Ok() )
	{
		Font.SetPointSize((int)(m_Scale * Font.GetPointSize()));

		SetFont(Font);
	}
}

//---------------------------------------------------------
void CSG_Map_DC::SetFont(const wxFont &Font)
{
	m_dc.SetFont(Font);

	if( m_dc_mask.IsOk() )
	{
		m_dc_mask.SetFont(Font);
	}
}

//---------------------------------------------------------
void CSG_Map_DC::SetPen(const wxPen &Pen)
{
	m_dc.SetPen(Pen);

	if( m_dc_mask.IsOk() )
	{
		wxPen _Pen(Pen); _Pen.SetColour(*wxWHITE);

		m_dc_mask.SetPen(_Pen);
	}
}

//---------------------------------------------------------
void CSG_Map_DC::SetBrush(const wxBrush &Brush)
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
void CSG_Map_DC::DrawPoint(int x, int y)
{
	m_dc.DrawPoint(x, y);

	if( m_dc_mask.IsOk() )
	{
		m_dc_mask.DrawPoint(x, y);
	}
}

//---------------------------------------------------------
void CSG_Map_DC::DrawLine(int x1, int y1, int x2, int y2)
{
	m_dc.DrawLine(x1, y1, x2, y2);

	if( m_dc_mask.IsOk() )
	{
		m_dc_mask.DrawLine(x1, y1, x2, y2);
	}
}

//---------------------------------------------------------
void CSG_Map_DC::DrawRectangle(int x, int y, int width, int height)
{
	m_dc.DrawRectangle(x, y, width, height);

	if( m_dc_mask.IsOk() )
	{
		m_dc_mask.DrawRectangle(x, y, width, height);
	}
}

void CSG_Map_DC::DrawRectangle(const wxRect &r)
{
	m_dc.DrawRectangle(r);

	if( m_dc_mask.IsOk() )
	{
		m_dc_mask.DrawRectangle(r);
	}
}

//---------------------------------------------------------
void CSG_Map_DC::DrawCircle(int x, int y, int radius)
{
	m_dc.DrawCircle(x, y, radius);

	if( m_dc_mask.IsOk() )
	{
		m_dc_mask.DrawCircle(x, y, radius);
	}
}

//---------------------------------------------------------
void CSG_Map_DC::DrawArc(int xStart, int yStart, int xEnd, int yEnd, int xc, int yc)
{
	m_dc.DrawArc(xStart, yStart, xEnd, yEnd, xc, yc);

	if( m_dc_mask.IsOk() )
	{
		m_dc_mask.DrawArc(xStart, yStart, xEnd, yEnd, xc, yc);
	}
}

//---------------------------------------------------------
void CSG_Map_DC::DrawPolygon(int n, const wxPoint points[], int xoffset, int yoffset, wxPolygonFillMode fill_style)
{
	m_dc.DrawPolygon(n, points, xoffset, yoffset, fill_style);

	if( m_dc_mask.IsOk() )
	{
		m_dc_mask.DrawPolygon(n, points, xoffset, yoffset, fill_style);
	}
}

//---------------------------------------------------------
void CSG_Map_DC::DrawPolyPolygon(int n, const int count[], const wxPoint points[], int xoffset, int yoffset, wxPolygonFillMode fill_style)
{
	m_dc.DrawPolyPolygon(n, count, points, xoffset, yoffset, fill_style);

	if( m_dc_mask.IsOk() )
	{
		m_dc_mask.DrawPolyPolygon(n, count, points, xoffset, yoffset, fill_style);
	}
}

//---------------------------------------------------------
void CSG_Map_DC::DrawBitmap(const wxBitmap &bitmap, int x, int y, bool useMask)
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
void CSG_Map_DC::DrawText(int Align, int x, int y, const wxString &Text)
{
	Draw_Text(m_dc, Align, x, y, Text);

	if( m_dc_mask.IsOk() )
	{
		Draw_Text(m_dc_mask, Align, x, y, Text);
	}
}

//---------------------------------------------------------
void CSG_Map_DC::DrawText(int Align, int x, int y, double Angle, const wxString &Text)
{
	Draw_Text(m_dc, Align, x, y, Angle, Text);

	if( m_dc_mask.IsOk() )
	{
		Draw_Text(m_dc_mask, Align, x, y, Angle, Text);
	}
}

//---------------------------------------------------------
void CSG_Map_DC::DrawText(int Align, int x, int y, const wxString &Text, int Effect, wxColour Effect_Color, int Effect_Size)
{
	Draw_Text(m_dc, Align, x, y, Text, Effect, Effect_Color, Effect_Size);

	if( m_dc_mask.IsOk() )
	{
		Draw_Text(m_dc_mask, Align, x, y, Text, Effect, Effect_Color, Effect_Size);
	}
}

//---------------------------------------------------------
void CSG_Map_DC::DrawText(int Align, int x, int y, double Angle, const wxString &Text, int Effect, wxColour Effect_Color, int Effect_Size)
{
	Draw_Text(m_dc, Align, x, y, Angle, Text, Effect, Effect_Color, Effect_Size);

	if( m_dc_mask.IsOk() )
	{
		Draw_Text(m_dc_mask, Align, x, y, Angle, Text, Effect, Effect_Color, Effect_Size);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
