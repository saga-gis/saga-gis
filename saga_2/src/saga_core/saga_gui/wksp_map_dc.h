
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
	double						xWorld2DC				(double x)		{	return(                     (int)(0.5 + (x - m_rWorld.Get_XMin()) * m_World2DC) );	}
	double						yWorld2DC				(double y)		{	return( m_rDC.GetHeight() - (int)(0.5 + (y - m_rWorld.Get_YMin()) * m_World2DC) );	}

	TSG_Point_Int					World2DC				(TSG_Point p)	{	TSG_Point_Int _p; _p.x = (int)xWorld2DC(p.x), _p.y = (int)yWorld2DC(p.y); return( _p );	}

	//-----------------------------------------------------
	void						Set_Font				(wxFont &Font);

	//-----------------------------------------------------
	void						Draw_Polygon			(CSG_Shape_Polygon *pPolygon);

	//-----------------------------------------------------
	bool						IMG_Draw_Begin			(double Transparency);
	bool						IMG_Draw_End			(void);

	BYTE						IMG_Get_Mask_Red		(void)	{	return( m_Mask_Red );	}
	BYTE						IMG_Get_Mask_Green		(void)	{	return( m_Mask_Green );	}
	BYTE						IMG_Get_Mask_Blue		(void)	{	return( m_Mask_Blue );	}

	//-----------------------------------------------------
	void						IMG_Set_Pixel_Direct	(int n, int Color)
	{
		if( n >= 0 && n < m_img_nBytes )
		{
			BYTE	r	= SG_GET_R(Color), g	= SG_GET_G(Color), b	= SG_GET_B(Color);
			double	d;

			switch( m_img_mode )
			{
			case IMG_MODE_OPAQUE: default:
				m_img_rgb[n + 0]	= r;
				m_img_rgb[n + 1]	= g;
				m_img_rgb[n + 2]	= b;
				break;

			case IMG_MODE_SHADING:
				d					= (r + g + b) / 3.0 / 256.0;
				m_img_rgb[n + 0]	= (Color = (int)(d * m_img_dc_rgb[n + 0])) > 255 ? 255 : Color;
				m_img_rgb[n + 1]	= (Color = (int)(d * m_img_dc_rgb[n + 1])) > 255 ? 255 : Color;
				m_img_rgb[n + 2]	= (Color = (int)(d * m_img_dc_rgb[n + 2])) > 255 ? 255 : Color;
				break;

			case IMG_MODE_TRANSPARENT:
				d					= 1.0 - m_Transparency;
				m_img_rgb[n + 0]	= (int)(d * r + m_Transparency * m_img_dc_rgb[n + 0]);
				m_img_rgb[n + 1]	= (int)(d * g + m_Transparency * m_img_dc_rgb[n + 1]);
				m_img_rgb[n + 2]	= (int)(d * b + m_Transparency * m_img_dc_rgb[n + 2]);
				break;

			case IMG_MODE_TRANSPARENT_ALPHA:
				if( (d = SG_GET_A(Color) / 256.0) < 1.0 )
				{
					m_img_rgb[n + 0]	= (int)((1.0 - d) * r + d * m_img_dc_rgb[n + 0]);
					m_img_rgb[n + 1]	= (int)((1.0 - d) * g + d * m_img_dc_rgb[n + 1]);
					m_img_rgb[n + 2]	= (int)((1.0 - d) * b + d * m_img_dc_rgb[n + 2]);
				}
				break;
			}
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

	BYTE						m_Mask_Red, m_Mask_Green, m_Mask_Blue;

	BYTE						*m_img_rgb, *m_img_dc_rgb;

	int							m_img_nx, m_img_nBytes, m_img_mode;

	double						m_Transparency;

	wxImage						m_img, m_img_dc;

	wxBitmap					dc_BMP;


	void						TEST_Draw_Polygon		(CSG_Shape_Polygon *pPolygon);
	void						TEST_Draw_Polygon_Line	(CSG_Grid &Mask, int ax, int ay, int bx, int by, bool bDirChanged);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef _HEADER_INCLUDED__SAGA_GUI__WKSP_Map_DC_H
