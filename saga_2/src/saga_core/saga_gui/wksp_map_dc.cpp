
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

#include "helper.h"

#include "wksp_map_dc.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Map_DC::CWKSP_Map_DC(const CGEO_Rect &rWorld, const wxRect &rDC, double Scale, int Background)
{
	m_rWorld		= rWorld;
	m_rDC			= rDC;
	m_Scale			= Scale;

	//-----------------------------------------------------
	double	d		= (double)m_rDC.GetWidth() / (double)m_rDC.GetHeight();

	if( d > m_rWorld.Get_XRange() / m_rWorld.Get_YRange() )
	{
		d	= (m_rWorld.Get_YRange() * d - m_rWorld.Get_XRange()) / 2.0;
		m_rWorld.Inflate(d, 0.0, false);
	}
	else if( d < m_rWorld.Get_XRange() / m_rWorld.Get_YRange() )
	{
		d	= (m_rWorld.Get_XRange() / d - m_rWorld.Get_YRange()) / 2.0;
		m_rWorld.Inflate(0.0, d, false);
	}

	//-----------------------------------------------------
	m_World2DC		= (double)m_rDC.GetWidth() / m_rWorld.Get_XRange();
	m_DC2World		= m_rWorld.Get_XRange() / (double)m_rDC.GetWidth();

	dc_BMP.Create(m_rDC.GetWidth(), m_rDC.GetHeight());
	dc.SelectObject(dc_BMP);
	dc.SetBackground(wxBrush(Get_Color_asWX(Background), wxSOLID));
	dc.Clear();

	m_img_rgb		= NULL;
	m_img_dc_rgb	= NULL;

	m_Mask_Red		= 1;
	m_Mask_Green	= 2;
	m_Mask_Blue		= 3;
}

//---------------------------------------------------------
CWKSP_Map_DC::~CWKSP_Map_DC(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
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
//														 //
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Map_DC::IMG_Draw_Begin(double Transparency)
{
	BYTE		*pRGB;
	int			i;
	wxBitmap	tmp_BMP;
	wxMemoryDC	tmp_dc;

	if( m_DC2World > 0.0 && m_rDC.GetWidth() > 0 && m_rDC.GetHeight() > 0 )
	{
		m_img.Create(m_rDC.GetWidth(), m_rDC.GetHeight());
		m_img.SetMask(true);
		m_img.SetMaskColour(m_Mask_Red, m_Mask_Green, m_Mask_Blue);
		m_img_rgb		= m_img.GetData();
		m_img_nx		= m_rDC.GetWidth();
		m_img_nBytes	= 3 * m_rDC.GetHeight() * m_rDC.GetWidth();

		for(i=0, pRGB=m_img_rgb; i<m_img_nBytes; i+=3)
		{
			*pRGB++	= m_Mask_Red;
			*pRGB++	= m_Mask_Green;
			*pRGB++	= m_Mask_Blue;
		}

		if( Transparency > 0.0 )
		{
			tmp_BMP.Create(m_rDC.GetWidth(), m_rDC.GetHeight());
			tmp_dc.SelectObject(tmp_BMP);
			tmp_dc.Blit(0, 0, m_rDC.GetWidth(), m_rDC.GetHeight(), &dc, 0, 0);
			tmp_dc.SelectObject(wxNullBitmap);

			m_img_dc		= tmp_BMP.ConvertToImage();
			m_img_dc_rgb	= m_img_dc.GetData();

			m_Transparency	= Transparency;
			m_img_mode		= Transparency <= 1.0 ? IMG_MODE_TRANSPARENT : (Transparency <= 2.0 ? IMG_MODE_SHADING : IMG_MODE_TRANSPARENT_ALPHA);
		}
		else
		{
			m_img_mode		= IMG_MODE_OPAQUE;
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Map_DC::IMG_Draw_End(void)
{
	if( m_img.Ok() )
	{
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

	return( false );
}

//---------------------------------------------------------
void CWKSP_Map_DC::IMG_Set_Rect(int x_a, int y_a, int x_b, int y_b, int Color)
{
	int		x, y, n;

	//-----------------------------------------------------
	if( y_a > y_b )
	{
		y	= y_a;	y_a	= y_b;	y_b	= y;
	}

	if( y_a <  0 )
	{
		y_a	=  0;
	}

	if( y_b >= m_rDC.GetHeight() )
	{
		y_b	=  m_rDC.GetHeight();
	}

	//-----------------------------------------------------
	if( x_a > x_b )
	{
		x	= x_a;	x_a	= x_b;	x_b	= x;
	}

	if( x_a <  0 )
	{
		x_a	=  0;
	}

	if( x_b >= m_rDC.GetWidth() )
	{
		x_b	=  m_rDC.GetWidth();
	}

	//-----------------------------------------------------
	for(y=y_a; y<y_b; y++)
	{
		n	= 3 * (y * m_img_nx + x_a);

		for(x=x_a; x<x_b; x++, n+=3)
		{
			IMG_Set_Pixel_Direct(n, Color);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Map_DC::Draw_Polygon(CShape_Polygon *pPolygon)
{
//	TEST_Draw_Polygon(pPolygon);	return;	// testing alternative polygon drawing

	int			iPart, iPoint, jPoint, *nPoints;
	TGEO_Point	p;
	wxPoint		*Points;

	//-----------------------------------------------------
	if( pPolygon->Get_Part_Count() == 1 )
	{
		if( pPolygon->Get_Point_Count(0) > 2 )
		{
			Points	= new wxPoint[pPolygon->Get_Point_Count(0)];

			for(iPoint=0; iPoint<pPolygon->Get_Point_Count(0); iPoint++)
			{
				p					= pPolygon->Get_Point(iPoint, 0);
				Points[iPoint].x	= (int)xWorld2DC(p.x);
				Points[iPoint].y	= (int)yWorld2DC(p.y);
			}

			dc.DrawPolygon(pPolygon->Get_Point_Count(0), Points);

			delete[](Points);
		}
	}

	//-----------------------------------------------------
	else if( pPolygon->Get_Part_Count() > 1 )
	{
		nPoints	= new int[pPolygon->Get_Part_Count()];

		for(iPart=0, jPoint=0; iPart<pPolygon->Get_Part_Count(); iPart++)
		{
			jPoint	+= (nPoints[iPart] = pPolygon->Get_Point_Count(iPart) > 2 ? pPolygon->Get_Point_Count(iPart) + 1 : 0);
		}

		Points	= new wxPoint[jPoint];

		for(iPart=0, jPoint=0; iPart<pPolygon->Get_Part_Count(); iPart++)
		{
			if( nPoints[iPart] > 0 )
			{
				for(iPoint=0; iPoint<pPolygon->Get_Point_Count(iPart); iPoint++)
				{
					p					= pPolygon->Get_Point(iPoint, iPart);
					Points[jPoint].x	= (int)xWorld2DC(p.x);
					Points[jPoint].y	= (int)yWorld2DC(p.y);
					jPoint++;
				}

				p					= pPolygon->Get_Point(0, iPart);
				Points[jPoint].x	= (int)xWorld2DC(p.x);
				Points[jPoint].y	= (int)yWorld2DC(p.y);
				jPoint++;
			}
		}

		dc.DrawPolyPolygon(pPolygon->Get_Part_Count(), nPoints, Points, 0, 0, wxODDEVEN_RULE);

		delete[](Points);
		delete[](nPoints);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Map_DC::TEST_Draw_Polygon(CShape_Polygon *pPolygon)
{
	IMG_Draw_Begin(0.5);

	int			iPart, iPoint, ix, iy, jx, jy, idy, jdy;
	TGEO_Point	p;
	CGrid		Mask(GRID_TYPE_Byte, m_rDC.GetWidth(), m_rDC.GetHeight());

	for(iPart=0; iPart<pPolygon->Get_Part_Count(); iPart++)
	{
		if( pPolygon->Get_Point_Count(iPart) > 2 )
		{
			p	= pPolygon->Get_Point(pPolygon->Get_Point_Count(iPart) - 2, iPart);
			jy	= (int)yWorld2DC(p.y);
			p	= pPolygon->Get_Point(pPolygon->Get_Point_Count(iPart) - 1, iPart);
			ix	= (int)xWorld2DC(p.x);
			iy	= (int)yWorld2DC(p.y);
			idy	= jy < iy ? -1 : (jy > iy ? 1 : 0);

			for(iPoint=0; iPoint<pPolygon->Get_Point_Count(iPart); iPoint++)
			{
				jx	= ix;
				jy	= iy;
				p	= pPolygon->Get_Point(iPoint, iPart);
				ix	= (int)xWorld2DC(p.x);
				iy	= (int)yWorld2DC(p.y);

				if( jy != iy )
				{
					jdy	= idy;
					idy	= jy < iy ? -1 : 1;

					TEST_Draw_Polygon_Line(Mask, ix, iy, jx, jy, jdy != idy);
				}
			}
		}
	}

	bool	bFill;
	int		x, y, Fill	= Get_Color_asInt(dc.GetBrush().GetColour());

	for(y=0; y<Mask.Get_NY(); y++)
	{
		for(x=0, bFill=false; x<Mask.Get_NX(); x++)
		{
			if( Mask.asInt(x, y) )
			{
				bFill	= !bFill;

				IMG_Set_Pixel(x, y, Fill);
			}
			else if( bFill )
			{
				IMG_Set_Pixel(x, y, Fill);
			}
		}
	}

	IMG_Draw_End();
}

//---------------------------------------------------------
void CWKSP_Map_DC::TEST_Draw_Polygon_Line(CGrid &Mask, int ax, int ay, int bx, int by, bool bDirChanged)
{
	int		x, y, dy, nx, ny;
	double	fx, dx;

	nx	= bx - ax;
	ny	= by - ay;

	if( ny != 0 )
	{
		if( ny < 0 )
		{
			dx	= -nx / (double)ny;
			dy	= -1;
		}
		else
		{
			dx	=  nx / (double)ny;
			dy	=  1;
		}

		for(y=ay, fx=ax+0.5; y!=by; y+=dy, fx+=dx)
		{
			if( y >= 0 && y < Mask.Get_NY() )
			{
				if( (x = (int)fx) < Mask.Get_NX() )
				{
					if( x < 0 )	x	= 0;
					Mask.Set_Value(x, y, Mask.asInt(x, y) == 0 ? 1 : 0);
				}
			}
		}

		if( bDirChanged && y >= 0 && y < Mask.Get_NY() )
		{
			if( (x = (int)fx) < Mask.Get_NX() )
			{
				if( x < 0 )	x	= 0;
				Mask.Set_Value(x, y, Mask.asInt(x, y) == 0 ? 1 : 0);
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
