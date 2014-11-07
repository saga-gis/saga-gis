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

	m_Background[0]	= SG_GET_R(Background);
	m_Background[1]	= SG_GET_G(Background);
	m_Background[2]	= SG_GET_B(Background);

	//-----------------------------------------------------
	if( m_rWorld.Get_XRange() == 0.0 || m_rWorld.Get_YRange() == 0.0 )
	{
		m_rWorld.Inflate(m_rWorld.Get_XRange() ? 0.0 : 1.0, m_rWorld.Get_YRange() ? 0.0 : 1.0, false);
	}

	//-----------------------------------------------------
	// ensure cellsize in x-/y-direction are identical...
	double	dxdyDC		= (double)m_rDC.GetWidth() / (double)m_rDC.GetHeight();
	double	dxdyWorld	= m_rWorld.Get_XRange() / m_rWorld.Get_YRange();

	if( dxdyDC > dxdyWorld )
	{
		m_rWorld.Inflate(0.5 * (m_rWorld.Get_YRange() * dxdyDC - m_rWorld.Get_XRange()), 0.0, false);
	}
	else if( dxdyDC < dxdyWorld )
	{
		m_rWorld.Inflate(0.0, 0.5 * (m_rWorld.Get_XRange() / dxdyDC - m_rWorld.Get_YRange()), false);
	}

	//-----------------------------------------------------
	m_World2DC		= (double)m_rDC.GetWidth() / m_rWorld.Get_XRange();
	m_DC2World		= 1.0 / m_World2DC;

	//-----------------------------------------------------
	dc_BMP.Create(m_rDC.GetWidth(), m_rDC.GetHeight());
	dc.SelectObject(dc_BMP);
	dc.SetBackground(wxBrush(wxColour(m_Background[0], m_Background[1], m_Background[2]), wxSOLID));
	dc.Clear();
}

//---------------------------------------------------------
CWKSP_Map_DC::~CWKSP_Map_DC(void)
{}


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
void CWKSP_Map_DC::Draw_DC(CWKSP_Map_DC &dc_Source, double Transparency)
{
	if( 1 )
	{
		BYTE		*src_rgb, *dst_rgb;
		int			i, n;
		double		d;
		wxImage		src_img, dst_img;
		wxBitmap	bmp;
		wxMemoryDC	mdc;

		//-------------------------------------------------
		n	= 3 * m_rDC.GetHeight() * m_rDC.GetWidth();
		d	= 1.0 - Transparency;

		bmp.Create(m_rDC.GetWidth(), m_rDC.GetHeight());

		mdc.SelectObject(bmp);
		mdc.Blit(0, 0, m_rDC.GetWidth(), m_rDC.GetHeight(), &dc, 0, 0);
		mdc.SelectObject(wxNullBitmap);
		dst_img	= bmp.ConvertToImage();
		dst_rgb	= dst_img.GetData();

		mdc.SelectObject(bmp);
		mdc.Blit(0, 0, m_rDC.GetWidth(), m_rDC.GetHeight(), &dc_Source.dc, 0, 0);
		mdc.SelectObject(wxNullBitmap);
		src_img	= bmp.ConvertToImage();
		src_rgb	= src_img.GetData();

		//-------------------------------------------------
		for(i=0; i<n; i+=3, src_rgb+=3, dst_rgb+=3)
		{
			if( src_rgb[0] != dc_Source.m_Background[0]
			||  src_rgb[1] != dc_Source.m_Background[1]
			||  src_rgb[2] != dc_Source.m_Background[2] )
			{
				dst_rgb[0]	= (int)(d * src_rgb[0] + Transparency * dst_rgb[0]);
				dst_rgb[1]	= (int)(d * src_rgb[1] + Transparency * dst_rgb[1]);
				dst_rgb[2]	= (int)(d * src_rgb[2] + Transparency * dst_rgb[2]);
			}
		}

		//-------------------------------------------------
		dc.DrawBitmap(wxBitmap(dst_img), 0, 0, true);
	}
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
		m_img.SetMaskColour(m_Background[0], m_Background[1], m_Background[2]);
		m_img_rgb		= m_img.GetData();
		m_img_nx		= m_rDC.GetWidth();
		m_img_nBytes	= 3 * m_rDC.GetHeight() * m_rDC.GetWidth();

		for(i=0, pRGB=m_img_rgb; i<m_img_nBytes; i+=3)
		{
			*pRGB++	= m_Background[0];
			*pRGB++	= m_Background[1];
			*pRGB++	= m_Background[2];
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
void CWKSP_Map_DC::Draw_Polygon(CSG_Shape_Polygon *pPolygon)
{
//	TEST_Draw_Polygon(pPolygon);	return;	// testing alternative polygon drawing

	//-----------------------------------------------------
	if(	m_World2DC * pPolygon->Get_Extent().Get_XRange() <= 2.0
	&&	m_World2DC * pPolygon->Get_Extent().Get_YRange() <= 2.0 )
	{
		dc.DrawPoint(
			(int)xWorld2DC(pPolygon->Get_Extent().Get_XCenter()),
			(int)yWorld2DC(pPolygon->Get_Extent().Get_YCenter())
		);
	}

	//-----------------------------------------------------
	else
	{
		int			iPart, iPoint, jPoint, *nPoints;
		TSG_Point	p;
		wxPoint		*Points;

		//-------------------------------------------------
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

		//-------------------------------------------------
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
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Map_DC::TEST_Draw_Polygon(CSG_Shape_Polygon *pPolygon)
{
	double		xa, y, ya, d;
	int			xDC, xaDC, xbDC, yDC, yaDC, ybDC, Fill, *cross, iPart, iPoint, i, n, j;
	TSG_Point	iPt, jPt, Pt, aPt, bPt;

	Fill	= Get_Color_asInt(dc.GetBrush().GetColour());
	d		= 1.0 / m_World2DC;
	xaDC	= (int)xWorld2DC(pPolygon->Get_Extent().Get_XMin());
	xbDC	= (int)xWorld2DC(pPolygon->Get_Extent().Get_XMax());
	yaDC	= (int)yWorld2DC(pPolygon->Get_Extent().Get_YMin());
	ybDC	= (int)yWorld2DC(pPolygon->Get_Extent().Get_YMax());

	aPt.x	= pPolygon->Get_Extent().Get_XMin() - 1.0;
	bPt.x	= pPolygon->Get_Extent().Get_XMax() + 1.0;
	xa		= pPolygon->Get_Extent().Get_XMin();
	ya		= pPolygon->Get_Extent().Get_YMin();

	n		= xbDC - xaDC + 1;
	cross	= (int *)SG_Malloc(n * sizeof(int));

	IMG_Draw_Begin(0.5);

	for(yDC=yaDC, y=ya; yDC>=ybDC; yDC--, y+=d)
	{
		if( yDC >= 0 && yDC < m_rDC.GetHeight() )
		{
			aPt.y	= bPt.y	= y;
			memset(cross, 0, n * sizeof(int));

			for(iPart=0; iPart<pPolygon->Get_Part_Count(); iPart++)
			{
				iPt	= pPolygon->Get_Point(pPolygon->Get_Point_Count(iPart) - 1, iPart);

				for(iPoint=0; iPoint<pPolygon->Get_Point_Count(iPart); iPoint++)
				{
					jPt	= iPt;
					iPt	= pPolygon->Get_Point(iPoint, iPart);

					if(	(iPt.y <= y && y <= jPt.y)
					||	(iPt.y >= y && y >= jPt.y) )
					{
						if( SG_Get_Crossing(Pt, aPt, bPt, iPt, jPt, true) )
						{
							i	= (int)xWorld2DC(Pt.x) - xaDC;

							if( i >= 0 && i < n )
							{
								cross[i]++;
							}
						}
					}
				}
			}

			for(i=0, j=0, xDC=xaDC; i<n; xDC++, i++)
			{
				j	+= cross[i];

				if( (j % 2) != 0 && xDC >= 0 && xDC < m_rDC.GetWidth() )
				{
					IMG_Set_Pixel(xDC, yDC, Fill);
				}
			}
		}
	}

	IMG_Draw_End();

	SG_Free(cross);
}

//---------------------------------------------------------
void CWKSP_Map_DC::TEST_Draw_Polygon_Line(CSG_Grid &Mask, int ax, int ay, int bx, int by, bool bDirChanged)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
