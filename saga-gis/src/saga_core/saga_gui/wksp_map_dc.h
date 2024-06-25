
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
	IMG_MODE_OPAQUE	= 0,
	IMG_MODE_SHADING,
	IMG_MODE_TRANSPARENT,
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
	CWKSP_Map_DC(const CSG_Rect &rWorld, const wxRect &rDC, double Scale = 1., int Background = -1, bool bMask = false);
	virtual ~CWKSP_Map_DC(void);

	//-----------------------------------------------------
	wxMemoryDC &				Get_DC					(void) { return( m_dc ); }

	void						SetFont					(const wxFont  &Font );
	void						SetPen					(const wxPen   &Pen  );
	void						SetBrush				(const wxBrush &Brush);

	void						DrawPoint				(int x, int y);
	void						DrawLine				(int x1, int y1, int x2, int y2);
	void						DrawRectangle			(int x1, int y1, int width, int height);
	void						DrawRectangle			(const wxRect &r);
	void						DrawCircle				(int x, int y, int radius);
	void						DrawArc					(int xStart, int yStart, int xEnd, int yEnd, int xc, int yc);
	void						DrawPolygon				(int n, const wxPoint points[], int xoffset = 0, int yoffset = 0, wxPolygonFillMode fill_style = wxODDEVEN_RULE);
	void						DrawPolyPolygon			(int n, const int count[], const wxPoint points[], int xoffset = 0, int yoffset = 0, wxPolygonFillMode fill_style = wxODDEVEN_RULE);
	void						DrawBitmap				(const wxBitmap &bitmap, int x, int y, bool useMask = false);

	void						DrawText				(int Align, int x, int y              , const wxString &Text);
	void						DrawText				(int Align, int x, int y, double Angle, const wxString &Text);
	void						DrawText				(int Align, int x, int y              , const wxString &Text, int Effect, wxColour Effect_Color, int Effect_Size = 1);
	void						DrawText				(int Align, int x, int y, double Angle, const wxString &Text, int Effect, wxColour Effect_Color, int Effect_Size = 1);

	//-----------------------------------------------------
	const CSG_Rect &			rWorld					(void) const { return( m_rWorld   ); }
	const   wxRect &			rDC						(void) const { return( m_rDC      ); }

	const double &				Scale					(void) const { return( m_Scale    ); }

	const double &				World2DC				(void) const { return( m_World2DC ); }

	TSG_Point_Int				World2DC				(const CSG_Point &p)
	{
		TSG_Point_Int _p; _p.x = (int)xWorld2DC(p.x), _p.y = (int)yWorld2DC(p.y);

		return( _p );
	}

	double						xWorld2DC				(double x, bool bRound = true)
	{
		x = (x - m_rWorld.Get_XMin()) * m_World2DC;

		return( bRound ? (int)(x < 0. ? x - 0.5 : x + 0.5) : x );
	}

	double						yWorld2DC				(double y, bool bRound = true)
	{
		y = (m_rWorld.Get_YMax() - y) * m_World2DC - 1;

		return( bRound ? (int)(y < 0. ? y - 0.5 : y + 0.5) : y );
	}

	const double &				DC2World				(void) const { return( m_DC2World ); }

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

	bool						Get_Bitmap				(wxBitmap &Bitmap);
	bool						Get_Image				(wxImage  &Image );

	//-----------------------------------------------------
	bool						Draw_Layer_Begin		(void);
	bool						Draw_Layer_End			(double Transparency = 0.);

	void						Draw_Polygon			(CSG_Shape_Polygon *pPolygon);

	//-----------------------------------------------------
	bool						Draw_Image_Begin		(double Transparency, int Mode = IMG_MODE_TRANSPARENT);
	bool						Draw_Image_End			(void);

	void						Draw_Image_Pixels		(int x1, int y1, int x2, int y2, int Color);
	void						Draw_Image_Pixel		(int x, int y, int Color)
	{
		if( x >= 0 && x < m_img_nx )
		{
			_Draw_Image_Pixel(3 * (y * m_img_nx + x), Color);
		}
	}

	//-----------------------------------------------------
	void						Draw					(wxDC &dc_Target);


private:

	BYTE						*m_img_rgb { NULL }, *m_img_mask { NULL }, m_Background[3];

	int							m_img_nx { 0 }, m_img_nBytes { 0 }, m_img_mode { 0 };

	double						m_World2DC, m_DC2World, m_Opacity, m_Scale;

	CSG_Rect					m_rWorld;

	wxRect						m_rDC;

	wxImage						m_img;

	wxBitmap					m_bmp, m_bmp_mask, m_layer_bmp, m_layer_mask;

	wxMemoryDC					m_dc, m_dc_mask;


	//-----------------------------------------------------
	void						_Draw_Image_Pixel		(int i, int Color)
	{
		if( i >= 0 && i < m_img_nBytes )
		{
			BYTE r = SG_GET_R(Color), g = SG_GET_G(Color), b = SG_GET_B(Color);

			switch( m_img_mode )
			{
			case IMG_MODE_OPAQUE: default: {
				break; }

			case IMG_MODE_SHADING: {
				r = (BYTE)(r * m_img_rgb[i + 0] / 255.);
				g = (BYTE)(g * m_img_rgb[i + 1] / 255.);
				b = (BYTE)(b * m_img_rgb[i + 2] / 255.);
				break; }

			case IMG_MODE_TRANSPARENT: {
				if( m_Opacity <= 0. ) { return; }
				if( m_Opacity <  1. )
				{
					r = (BYTE)(r * m_Opacity + (1. - m_Opacity) * m_img_rgb[i + 0]);
					g = (BYTE)(g * m_Opacity + (1. - m_Opacity) * m_img_rgb[i + 1]);
					b = (BYTE)(b * m_Opacity + (1. - m_Opacity) * m_img_rgb[i + 2]);
				}
				break; }

			case IMG_MODE_TRANSPARENT_ALPHA: {
				double Opacity = m_Opacity * SG_GET_A(Color) / 255.;

				if( Opacity <= 0. ) { return; }
				if( Opacity <  1. )
				{
					r = (BYTE)(r *   Opacity + (1. -   Opacity) * m_img_rgb[i + 0]);
					g = (BYTE)(g *   Opacity + (1. -   Opacity) * m_img_rgb[i + 1]);
					b = (BYTE)(b *   Opacity + (1. -   Opacity) * m_img_rgb[i + 2]);
				}
				break; }
			}

			m_img_rgb[i + 0] = r;
			m_img_rgb[i + 1] = g;
			m_img_rgb[i + 2] = b;

			if( m_img_mask )
			{
				m_img_mask[i] = 1;
			}
		}
	}

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Map_DC_H
