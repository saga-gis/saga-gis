
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
//                    WKSP_Map_DC.h                      //
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
#ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Map_DC_H
#define _HEADER_INCLUDED__SAGA_GUI__WKSP_Map_DC_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/dcmemory.h>
#include <wx/image.h>

#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	IMG_MODE_OPAQUE			= 0,
	IMG_MODE_TRANSPARENT,
	IMG_MODE_SHADING,
	IMG_MODE_TRANSPARENT_ALPHA
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CWKSP_Map_DC
{
public:
	CWKSP_Map_DC(const CSG_Rect &rWorld, const wxRect &rDC, double Scale, int Background);
	virtual ~CWKSP_Map_DC(void);

	double						m_World2DC, m_DC2World, m_Scale;

	CSG_Rect					m_rWorld;

	wxRect						m_rDC;

	wxMemoryDC					dc;


	//-----------------------------------------------------
	double						xWorld2DC				(double x, bool bRound = true)
	{
		x	= (x - m_rWorld.Get_XMin()) * m_World2DC;

		return( bRound ? (int)(x < 0. ? x - 0.5 : x + 0.5) : x );
	}

	double						yWorld2DC				(double y, bool bRound = true)
	{
		y	= (m_rWorld.Get_YMax() - y) * m_World2DC - 1;

		return( bRound ? (int)(y < 0. ? y - 0.5 : y + 0.5) : y );
	}

	TSG_Point_Int				World2DC				(TSG_Point p)	{	TSG_Point_Int _p; _p.x = (int)xWorld2DC(p.x), _p.y = (int)yWorld2DC(p.y); return( _p );	}

	//-----------------------------------------------------
	double						xDC2World				(double x)
	{
		return( m_rWorld.Get_XMin() + m_DC2World * x );
	}

	double						yDC2World				(double y)
	{
		return( m_rWorld.Get_YMax() - m_DC2World * (y + 1) );
	}

	//-----------------------------------------------------
	void						Set_Font				(wxFont &Font);

	//-----------------------------------------------------
	void						Draw_DC					(CWKSP_Map_DC &dc_Source, double Transparency);
	void						Draw_Polygon			(CSG_Shape_Polygon *pPolygon);

	//-----------------------------------------------------
	bool						IMG_Draw_Begin			(double Transparency);
	bool						IMG_Draw_End			(void);

	//-----------------------------------------------------
	void						IMG_Set_Pixel_Direct	(int n, int Color)
	{
		if( n >= 0 && n < m_img_nBytes )
		{
			BYTE r = SG_GET_R(Color);
			BYTE g = SG_GET_G(Color);
			BYTE b = SG_GET_B(Color);

			switch( m_img_mode )
			{
			case IMG_MODE_OPAQUE: default: {
				} break;

			case IMG_MODE_SHADING: {
				r = (int)(r * m_img_dc_rgb[n + 0] / 255.);
				g = (int)(g * m_img_dc_rgb[n + 1] / 255.);
				b = (int)(b * m_img_dc_rgb[n + 2] / 255.);
				} break;

			case IMG_MODE_TRANSPARENT: {
				r = (int)(r * (1. - m_Transparency) + m_Transparency * m_img_dc_rgb[n + 0]);
				g = (int)(g * (1. - m_Transparency) + m_Transparency * m_img_dc_rgb[n + 1]);
				b = (int)(b * (1. - m_Transparency) + m_Transparency * m_img_dc_rgb[n + 2]);
				} break;

			case IMG_MODE_TRANSPARENT_ALPHA: {
				double	Alpha	= SG_GET_A(Color);

				if( Alpha >= 0. && Alpha < 1. )
				{
					r = (int)(r * (1. - Alpha) + Alpha * m_img_dc_rgb[n + 0]);
					g = (int)(g * (1. - Alpha) + Alpha * m_img_dc_rgb[n + 1]);
					b = (int)(b * (1. - Alpha) + Alpha * m_img_dc_rgb[n + 2]);
				}
				break; }
			}

			m_img_rgb[n + 0] = r;
			m_img_rgb[n + 1] = g;
			m_img_rgb[n + 2] = b;
		}
	}

	void						IMG_Set_Pixel			(int n, int Color)
	{
		IMG_Set_Pixel_Direct(3 * n, Color);
	}

	void						IMG_Set_Pixel			(int x, int y, int Color)
	{
		if( x >= 0 && x < m_img_nx )
		{
			IMG_Set_Pixel_Direct(3 * (y * m_img_nx + x), Color);
		}
	}

	void						IMG_Set_Rect			(int x_a, int y_a, int x_b, int y_b, int Color);

	//-----------------------------------------------------
	void						Draw					(wxDC &dc_Target);


private:

	BYTE						*m_img_rgb, *m_img_dc_rgb, m_Background[3];

	int							m_img_nx, m_img_nBytes, m_img_mode;

	double						m_Transparency;

	wxImage						m_img, m_img_dc;

	wxBitmap					dc_BMP;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Map_DC_H
