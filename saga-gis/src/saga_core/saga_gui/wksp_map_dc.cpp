
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

#include "helper.h"

#include "wksp_map_dc.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Map_DC::CWKSP_Map_DC(const CSG_Rect &rWorld, const wxRect &rDC, double Scale, int Background)
{
	m_rWorld		= rWorld;
	m_rDC			= rDC;
	m_Scale			= Scale;

	m_img_rgb		= NULL;
	m_img_dc_rgb	= NULL;

	m_Background[0]	= Background >= 0 ? SG_GET_R(Background) : 254;
	m_Background[1]	= Background >= 0 ? SG_GET_G(Background) : 255;
	m_Background[2]	= Background >= 0 ? SG_GET_B(Background) : 255;

	//-----------------------------------------------------
	if( m_rWorld.Get_XRange() == 0. || m_rWorld.Get_YRange() == 0. )
	{
		m_rWorld.Inflate(m_rWorld.Get_XRange() ? 0. : 1., m_rWorld.Get_YRange() ? 0. : 1., false);
	}

	//-----------------------------------------------------
	// ensure cellsize in x-/y-direction are identical...
	double	dxdyDC		= (double)m_rDC.GetWidth() / (double)m_rDC.GetHeight();
	double	dxdyWorld	= m_rWorld.Get_XRange() / m_rWorld.Get_YRange();

	if( dxdyDC > dxdyWorld )
	{
		m_rWorld.Inflate(0.5 * (m_rWorld.Get_YRange() * dxdyDC - m_rWorld.Get_XRange()), 0., false);
	}
	else if( dxdyDC < dxdyWorld )
	{
		m_rWorld.Inflate(0., 0.5 * (m_rWorld.Get_XRange() / dxdyDC - m_rWorld.Get_YRange()), false);
	}

	//-----------------------------------------------------
	m_World2DC		= (double)m_rDC.GetWidth() / m_rWorld.Get_XRange();
	m_DC2World		= 1. / m_World2DC;

	//-----------------------------------------------------
	dc_BMP.Create(m_rDC.GetWidth(), m_rDC.GetHeight());
	dc.SelectObject(dc_BMP);
	dc.SetBackground(wxBrush(wxColour(m_Background[0], m_Background[1], m_Background[2]), wxBRUSHSTYLE_SOLID));
	dc.Clear();
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
	dc.SelectObject(wxNullBitmap);

	dc_Target.DrawBitmap(dc_BMP, m_rDC.GetLeft(), m_rDC.GetTop(), false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Map_DC::Draw_DC(CWKSP_Map_DC &dc_Source, double Transparency)
{
	wxMemoryDC	mdc;
	wxBitmap	bmp(m_rDC.GetWidth(), m_rDC.GetHeight());

	mdc.SelectObject(bmp);
	mdc.Blit(0, 0, m_rDC.GetWidth(), m_rDC.GetHeight(), &          dc, 0, 0);
	mdc.SelectObject(wxNullBitmap);
	wxImage dst_img	= bmp.ConvertToImage();
	BYTE   *dst_rgb	= dst_img.GetData();

	mdc.SelectObject(bmp);
	mdc.Blit(0, 0, m_rDC.GetWidth(), m_rDC.GetHeight(), &dc_Source.dc, 0, 0);
	mdc.SelectObject(wxNullBitmap);
	wxImage src_img	= bmp.ConvertToImage();
	BYTE   *src_rgb	= src_img.GetData();

	//-----------------------------------------------------
	int	n	= m_rDC.GetHeight() * m_rDC.GetWidth();

	for(int i=0; i<n; i++, src_rgb+=3, dst_rgb+=3)
	{
		if( src_rgb[0] != dc_Source.m_Background[0]
		||  src_rgb[1] != dc_Source.m_Background[1]
		||  src_rgb[2] != dc_Source.m_Background[2] )
		{
			dst_rgb[0]	= (int)((1. - Transparency) * src_rgb[0] + Transparency * dst_rgb[0]);
			dst_rgb[1]	= (int)((1. - Transparency) * src_rgb[1] + Transparency * dst_rgb[1]);
			dst_rgb[2]	= (int)((1. - Transparency) * src_rgb[2] + Transparency * dst_rgb[2]);
		}
	}

	//-----------------------------------------------------
	dc.DrawBitmap(wxBitmap(dst_img), 0, 0, true);
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

		dc.SetFont(Font);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Map_DC::IMG_Draw_Begin(double Transparency, int Mode)
{
	if( m_DC2World <= 0. || m_rDC.GetWidth() <= 0 || m_rDC.GetHeight() <= 0 )
	{
		return( false );
	}

	m_Opacity	= Transparency < 0. ? 1. : Transparency > 1. ? 0. : 1. - Transparency;

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
	m_img.Create(m_rDC.GetWidth(), m_rDC.GetHeight());
	m_img.SetMask(true);
	m_img.SetMaskColour(m_Background[0], m_Background[1], m_Background[2]);
	m_img_rgb		= m_img.GetData();
	m_img_nx		= m_rDC.GetWidth();
	m_img_nBytes	= 3 * m_rDC.GetHeight() * m_rDC.GetWidth();

	BYTE	*pRGB	= m_img_rgb;

	for(int i=0; i<m_img_nBytes; i+=3)
	{
		*pRGB++	= m_Background[0];
		*pRGB++	= m_Background[1];
		*pRGB++	= m_Background[2];
	}

	//-----------------------------------------------------
	if( m_img_mode != IMG_MODE_OPAQUE )
	{
		wxBitmap	tmp_BMP(m_rDC.GetWidth(), m_rDC.GetHeight());
		wxMemoryDC	tmp_dc;

		tmp_dc.SelectObject(tmp_BMP);
		tmp_dc.Blit(0, 0, m_rDC.GetWidth(), m_rDC.GetHeight(), &dc, 0, 0);
		tmp_dc.SelectObject(wxNullBitmap);

		m_img_dc		= tmp_BMP.ConvertToImage();
		m_img_dc_rgb	= m_img_dc.GetData();
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Map_DC::IMG_Draw_End(void)
{
	if( !m_img.Ok() )
	{
		return( false );
	}

	//-----------------------------------------------------
	dc.DrawBitmap(wxBitmap(m_img), 0, 0, true);

	m_img.Destroy();
	m_img_rgb		= NULL;
	m_img_nx		= 0;
	m_img_nBytes	= 0;

	if( m_img_dc.Ok() )
	{
		m_img_dc.Destroy();
		m_img_dc_rgb	= NULL;
	}

	return( true );
}

//---------------------------------------------------------
void CWKSP_Map_DC::IMG_Set_Rect(int x_a, int y_a, int x_b, int y_b, int Color)
{
	if( y_a >  y_b               ) { double y = y_a; y_a = y_b; y_b = y; }
	if( y_a <  0                 ) { y_a = 0; }
	if( y_b >= m_rDC.GetHeight() ) { y_b = m_rDC.GetHeight(); }

	if( x_a > x_b                ) { double x = x_a; x_a = x_b; x_b = x; }
	if( x_a < 0                  ) { x_a = 0; }
	if( x_b >= m_rDC.GetWidth () ) { x_b = m_rDC.GetWidth (); }

	//-----------------------------------------------------
	for(int y=y_a; y<y_b; y++)
	{
		int	n	= 3 * (y * m_img_nx + x_a);

		for(int x=x_a; x<x_b; x++, n+=3)
		{
			IMG_Set_Pixel_Direct(n, Color);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Map_DC::Draw_Polygon(CSG_Shape_Polygon *pPolygon)
{
	if(	m_World2DC * pPolygon->Get_Extent().Get_XRange() <= 2.
	&&	m_World2DC * pPolygon->Get_Extent().Get_YRange() <= 2. )
	{
		dc.DrawPoint(
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

		dc.DrawPolygon(pPolygon->Get_Point_Count(), Points);

		delete[](Points);
	}

	//-----------------------------------------------------
	else if( pPolygon->Get_Part_Count() > 1 )
	{
		wxPoint  *Points = new wxPoint[pPolygon->Get_Point_Count()];
		int     *nPoints = new int    [pPolygon->Get_Part_Count ()];

		for(int iPart=0, jPoint=0; iPart<pPolygon->Get_Part_Count(); iPart++)
		{
			nPoints[iPart]	= pPolygon->Get_Point_Count(iPart);

			for(int iPoint=0; iPoint<pPolygon->Get_Point_Count(iPart); iPoint++, jPoint++)
			{
				Points[jPoint].x = (int)xWorld2DC(pPolygon->Get_Point(iPoint, iPart).x);
				Points[jPoint].y = (int)yWorld2DC(pPolygon->Get_Point(iPoint, iPart).y);
			}
		}

		dc.DrawPolyPolygon(pPolygon->Get_Part_Count(), nPoints, Points, 0, 0, wxODDEVEN_RULE);

		delete[]( Points);
		delete[](nPoints);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
