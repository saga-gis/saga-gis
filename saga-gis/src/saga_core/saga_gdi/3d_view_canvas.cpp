/**********************************************************
 * Version $Id: 3d_view_canvas.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

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
//                   3d_view_canvas.cpp                  //
//                                                       //
//                 Copyright (C) 2014 by                 //
//                      Olaf Conrad                      //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "3d_view_tools.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	COLOR_MODE_RGB,
	COLOR_MODE_RED,
	COLOR_MODE_GREEN,
	COLOR_MODE_BLUE,
	COLOR_MODE_CYAN
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_3DView_Canvas::CSG_3DView_Canvas(void)
{
	m_pDrape		= NULL;
	m_Image_pRGB	= NULL;

	m_bgColor		= SG_COLOR_WHITE;
	m_bBox			= true;
	m_bStereo		= false;
	m_dStereo		= 2.0;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_3DView_Canvas::Set_Image(BYTE *pRGB, int NX, int NY)
{
	m_Image_pRGB	= pRGB;

	m_Image_NX		= NX;
	m_Image_NY		= NY;

	m_Image_zMax.Create(m_Image_NX, m_Image_NY);

	m_Projector.Set_Screen(m_Image_NX, m_Image_NY);
}

//---------------------------------------------------------
bool CSG_3DView_Canvas::Draw(void)
{
	if( !m_Image_pRGB || m_Image_NX < 1 || m_Image_NY < 1 )
	{
		return( false );
	}

	_Draw_Background();

	//-------------------------------------------------
	if( m_Data_Min.x >= m_Data_Max.x
	||  m_Data_Min.y >= m_Data_Max.y
	||  m_Data_Min.z >  m_Data_Max.z )
	{
		return( false );
	}

	//-------------------------------------------------
	if( !On_Before_Draw() )
	{
		return( false );
	}

	//-------------------------------------------------
	m_Projector.Set_Center(
		m_Data_Min.x + 0.5 * (m_Data_Max.x - m_Data_Min.x),
		m_Data_Min.y + 0.5 * (m_Data_Max.y - m_Data_Min.y),
		m_Data_Min.z + 0.5 * (m_Data_Max.z - m_Data_Min.z)
	);

	m_Projector.Set_Scale(
		 (m_Image_NX / (double)m_Image_NY) > ((m_Data_Max.x - m_Data_Min.x) / (m_Data_Max.y - m_Data_Min.y))
		? m_Image_NX / (m_Data_Max.x - m_Data_Min.x)
		: m_Image_NY / (m_Data_Max.y - m_Data_Min.y)
	);

	//-------------------------------------------------
	if( m_bStereo == false )
	{
		m_Image_zMax.Assign(999999.0);

		m_Color_Mode	= COLOR_MODE_RGB;

		On_Draw();

		_Draw_Box();
	}

	//-----------------------------------------------------
	else
	{
		double	dRotate	= M_DEG_TO_RAD * 0.5 * m_dStereo;
		double	yRotate	= m_Projector.Get_yRotation();
		double	dShift	= -0.01 * m_Image_NX;
		double	xShift	= m_Projector.Get_xShift();

		//-------------------------------------------------
		m_Image_zMax.Assign(999999.0);

		m_Projector.Set_xShift   (xShift  - dShift );
		m_Projector.Set_yRotation(yRotate - dRotate);

		m_Color_Mode	= COLOR_MODE_RED;

		On_Draw();

		_Draw_Box();

		//-------------------------------------------------
		m_Image_zMax.Assign(999999.0);

		m_Projector.Set_xShift   (xShift  + dShift );
		m_Projector.Set_yRotation(yRotate + dRotate);

		m_Color_Mode	= COLOR_MODE_CYAN;

		On_Draw();

		_Draw_Box();

		//-------------------------------------------------
		m_Projector.Set_xShift   (xShift );
		m_Projector.Set_yRotation(yRotate);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_3DView_Canvas::_Draw_Background(void)
{
	BYTE	r, g, b;

	if( m_bStereo )	// greyscale
	{
		r	= g	= b	= (int)((SG_GET_R(m_bgColor) + SG_GET_G(m_bgColor) + SG_GET_B(m_bgColor)) / 3.0);
	}
	else
	{
		r	= SG_GET_R(m_bgColor);
		g	= SG_GET_G(m_bgColor);
		b	= SG_GET_B(m_bgColor);
	}

	#pragma omp parallel for
	for(int y=0; y<m_Image_NY; y++)
	{
		BYTE	*pRGB	= m_Image_pRGB + y * 3 * m_Image_NX;

		for(int x=0; x<m_Image_NX; x++)
		{
			*pRGB	= r;	pRGB++;
			*pRGB	= g;	pRGB++;
			*pRGB	= b;	pRGB++;
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_3DView_Canvas::_Draw_Box(void)
{
	if( !m_bBox )
	{
		return;
	}

	int	color	= SG_GET_RGB(SG_GET_R(m_bgColor) + 128, SG_GET_G(m_bgColor) + 128, SG_GET_B(m_bgColor) + 128);

	TSG_Point_Z	p[2][4], Buffer;

	Buffer.x	= 0.01 * (m_Data_Max.x - m_Data_Min.x);
	Buffer.y	= 0.01 * (m_Data_Max.y - m_Data_Min.y);
	Buffer.z	= 0.01 * (m_Data_Max.z - m_Data_Min.z);

	for(int i=0; i<2; i++)
	{
		p[i][0].x	= m_Data_Min.x - Buffer.x;	p[i][0].y	= m_Data_Min.y - Buffer.y;
		p[i][1].x	= m_Data_Max.x + Buffer.x;	p[i][1].y	= m_Data_Min.y - Buffer.y;
		p[i][2].x	= m_Data_Max.x + Buffer.x;	p[i][2].y	= m_Data_Max.y + Buffer.y;
		p[i][3].x	= m_Data_Min.x - Buffer.x;	p[i][3].y	= m_Data_Max.y + Buffer.y;
		p[i][0].z	= p[i][1].z = p[i][2].z = p[i][3].z = i == 0
			? m_Data_Min.z - Buffer.z
			: m_Data_Max.z + Buffer.z;

		for(int j=0; j<4; j++)
		{
			m_Projector.Get_Projection(p[i][j]);
		}

		Draw_Line(p[i][0], p[i][1], color);
		Draw_Line(p[i][1], p[i][2], color);
		Draw_Line(p[i][2], p[i][3], color);
		Draw_Line(p[i][3], p[i][0], color);
	}

	Draw_Line(p[0][0], p[1][0], color);
	Draw_Line(p[0][1], p[1][1], color);
	Draw_Line(p[0][2], p[1][2], color);
	Draw_Line(p[0][3], p[1][3], color);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_3DView_Canvas::Get_Color(double Value)
{
	return( (int)Value );
}

//---------------------------------------------------------
int CSG_3DView_Canvas::Dim_Color(int Color, double dim)
{
	if( dim <= 0.0 )
	{
		return( m_bgColor );
	}

	if( dim != 1.0 )
	{
		int	r	= SG_GET_R(m_bgColor) + (int)(dim * (SG_GET_R(Color) - SG_GET_R(m_bgColor))); if( r > 255 ) r = 255;
		int	g	= SG_GET_G(m_bgColor) + (int)(dim * (SG_GET_G(Color) - SG_GET_G(m_bgColor))); if( g > 255 ) g = 255;
		int	b	= SG_GET_B(m_bgColor) + (int)(dim * (SG_GET_B(Color) - SG_GET_B(m_bgColor))); if( b > 255 ) b = 255;

		Color	= SG_GET_RGB(r, g, b);
	}

	return( Color );
}

//---------------------------------------------------------
int CSG_3DView_Canvas::_Dim_Color(int Color, double dim)
{
	if( dim <= 0.0 )
	{
		return( 0 );
	}

	if( dim != 1.0 )
	{
		int	r	= (int)(dim * SG_GET_R(Color)); if( r > 255 ) r = 255;
		int	g	= (int)(dim * SG_GET_G(Color)); if( g > 255 ) g = 255;
		int	b	= (int)(dim * SG_GET_B(Color)); if( b > 255 ) b = 255;

		Color	= SG_GET_RGB(r, g, b);
	}

	return( Color );
}

//---------------------------------------------------------
inline void CSG_3DView_Canvas::_Draw_Pixel(int x, int y, double z, int color)
{
	if( x >= 0 && x < m_Image_NX && y >= 0 && y < m_Image_NY && z < m_Image_zMax[y][x] )
	{
		BYTE	*RGB	= m_Image_pRGB + 3 * (y * m_Image_NX + x);

		switch( m_Color_Mode )
		{
		case COLOR_MODE_RGB:
			RGB[0]	= SG_GET_R(color);
			RGB[1]	= SG_GET_G(color);
			RGB[2]	= SG_GET_B(color);
			break;

		case COLOR_MODE_RED:
			RGB[0]	= (SG_GET_R(color) + SG_GET_G(color) + SG_GET_B(color)) / 3;
			break;

		case COLOR_MODE_GREEN:
			RGB[1]	= (SG_GET_R(color) + SG_GET_G(color) + SG_GET_B(color)) / 3;
			break;

		case COLOR_MODE_BLUE:
			RGB[2]	= (SG_GET_R(color) + SG_GET_G(color) + SG_GET_B(color)) / 3;
			break;

		case COLOR_MODE_CYAN:
			RGB[1]	= 
			RGB[2]	= (SG_GET_R(color) + SG_GET_G(color) + SG_GET_B(color)) / 3;
			break;
		}

		m_Image_zMax[y][x]	= z;
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_3DView_Canvas::Draw_Point(int x, int y, double z, int color, int size)
{
	if( z > 0.0 )
	{
		_Draw_Pixel(x, y, z, color);

		if( size > 0 && size < 50 )
		{
			for(int iy=1; iy<=size; iy++)
			{
				for(int ix=0; ix<=size; ix++)
				{
					if( ix*ix + iy*iy <= size*size )
					{
						_Draw_Pixel(x + ix, y + iy, z, color);
						_Draw_Pixel(x + iy, y - ix, z, color);
						_Draw_Pixel(x - ix, y - iy, z, color);
						_Draw_Pixel(x - iy, y + ix, z, color);
					}
				}
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_3DView_Canvas::Draw_Line(double ax, double ay, double az, double bx, double by, double bz, int aColor, int bColor)
{
	if(	(ax < 0 && bx < 0) || (ax >= m_Image_NX && bx >= m_Image_NX)
	||	(ay < 0 && by < 0) || (ay >= m_Image_NY && by >= m_Image_NY) )
	{
		return;
	}

	double	dx	= bx - ax;
	double	dy	= by - ay;
	double	dz	= bz - az;

	if( bz < 0.0 || az < 0.0 )
	{
		return;
	}

	double	n;

	if( fabs(dx) > fabs(dy) && fabs(dx) > 0.0 )
	{
		n	 = fabs(dx);
		dx	 = dx < 0 ? -1 : 1;
		dy	/= n;
		dz	/= n;
	}
	else if( fabs(dy) > 0.0 )
	{
		n	 = fabs(dy);
		dx	/= n;
		dy	 = dy < 0 ? -1 : 1;
		dz	/= n;
	}
	else
	{
		_Draw_Pixel((int)ax, (int)ay, az, aColor);
		_Draw_Pixel((int)bx, (int)by, bz, bColor);

		return;
	}

	//-----------------------------------------------------
	CSG_Colors	Colors(2); Colors[0] = aColor; Colors[1] = bColor;

	//-----------------------------------------------------
	for(double i=0.0; i<=n; i++, ax+=dx, ay+=dy, az+=dz)
	{
		_Draw_Pixel((int)ax, (int)ay, az, Colors.Get_Interpolated(i / n));
	}
}

void CSG_3DView_Canvas::Draw_Line(const TSG_Point_Z &a, const TSG_Point_Z &b, int aColor, int bColor)
{
	Draw_Line(a.x, a.y, a.z, b.x, b.y, b.z, aColor, bColor);
}

//---------------------------------------------------------
void CSG_3DView_Canvas::Draw_Line(double ax, double ay, double az, double bx, double by, double bz, int Color)
{
	if(	(ax < 0 && bx < 0) || (ax >= m_Image_NX && bx >= m_Image_NX)
	||	(ay < 0 && by < 0) || (ay >= m_Image_NY && by >= m_Image_NY) )
	{
		return;
	}

	double	dx	= bx - ax;
	double	dy	= by - ay;
	double	dz	= bz - az;

	if( bz < 0.0 || az < 0.0 )
	{
		return;
	}

	double	n;

	if( fabs(dx) > fabs(dy) && fabs(dx) > 0.0 )
	{
		n	 = fabs(dx);
		dx	 = dx < 0 ? -1 : 1;
		dy	/= n;
		dz	/= n;
	}
	else if( fabs(dy) > 0.0 )
	{
		n	 = fabs(dy);
		dx	/= n;
		dy	 = dy < 0 ? -1 : 1;
		dz	/= n;
	}
	else
	{
		_Draw_Pixel((int)ax, (int)ay, az, Color);

		return;
	}

	//-----------------------------------------------------
	for(double i=0.0; i<=n; i++, ax+=dx, ay+=dy, az+=dz)
	{
		_Draw_Pixel((int)ax, (int)ay, az, Color);
	}
}

void CSG_3DView_Canvas::Draw_Line(const TSG_Point_Z &a, const TSG_Point_Z &b, int Color)
{
	Draw_Line(a.x, a.y, a.z, b.x, b.y, b.z, Color);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_3DView_Canvas::Draw_Triangle(TSG_Triangle_Node p[3], double Light_Dec, double Light_Azi)
{
	double	A	= p[0].z * (p[1].x - p[2].x) + p[1].z * (p[2].x - p[0].x) + p[2].z * (p[0].x - p[1].x);
	double	B	= p[0].y * (p[1].z - p[2].z) + p[1].y * (p[2].z - p[0].z) + p[2].y * (p[0].z - p[1].z);
	double	C	= p[0].x * (p[1].y - p[2].y) + p[1].x * (p[2].y - p[0].y) + p[2].x * (p[0].y - p[1].y);

	double	s, a;

	if( C != 0.0 )
	{
		A	= - A / C;
		B	= - B / C;

		s	= M_PI_090 - atan(sqrt(A*A + B*B));
		a	= A != 0.0 ? M_PI_180 + atan2(B, A) : B > 0.0 ? M_PI_270 : (B < 0.0 ? M_PI_090 : -1.0);
	}
	else
	{
		s	= 0.0;
		a	= 0.0;
	}

	Draw_Triangle(p, (acos(sin(s) * sin(Light_Dec) + cos(s) * cos(Light_Dec) * cos(a - Light_Azi))) / M_PI_090);
}

//---------------------------------------------------------
void CSG_3DView_Canvas::Draw_Triangle(TSG_Triangle_Node p[3], double dim)
{
	if( p[0].z < 0.0 || p[1].z < 0.0 || p[2].z < 0.0 )
	{
		return;
	}

	//-----------------------------------------------------
	if( p[1].y < p[0].y ) {	TSG_Triangle_Node pp = p[1]; p[1] = p[0]; p[0] = pp;	}
	if( p[2].y < p[0].y ) {	TSG_Triangle_Node pp = p[2]; p[2] = p[0]; p[0] = pp;	}
	if( p[2].y < p[1].y ) {	TSG_Triangle_Node pp = p[2]; p[2] = p[1]; p[1] = pp;	}

	//-----------------------------------------------------
	TSG_Rect	r;

	r.yMin	= p[0].y;
	r.yMax	= p[2].y;
	r.xMin	= p[0].x < p[1].x ? (p[0].x < p[2].x ? p[0].x : p[2].x) : (p[1].x < p[2].x ? p[1].x : p[2].x);
	r.xMax	= p[0].x > p[1].x ? (p[0].x > p[2].x ? p[0].x : p[2].x) : (p[1].x > p[2].x ? p[1].x : p[2].x);

	if( r.yMin >= r.yMax || r.xMin >= r.xMax )
	{
		return;	// no area
	}

	if( (r.yMin < 0.0 && r.yMax < 0.0) || (r.yMin >= m_Image_NY && r.yMax >= m_Image_NY)
	||	(r.xMin < 0.0 && r.xMax < 0.0) || (r.xMin >= m_Image_NX && r.xMax >= m_Image_NX) )
	{
		return;	// completely outside grid
	}

	//-----------------------------------------------------
	TSG_Triangle_Node	q[3];

	if( (q[0].y	= p[2].y - p[0].y) > 0.0 )
	{
		q[0].x	= (p[2].x - p[0].x) / q[0].y;
		q[0].z	= (p[2].z - p[0].z) / q[0].y;
		q[0].c	= (p[2].c - p[0].c) / q[0].y;
		q[0].d	= (p[2].d - p[0].d) / q[0].y;
	}

	if( (q[1].y	= p[1].y - p[0].y) > 0.0 )
	{
		q[1].x	= (p[1].x - p[0].x) / q[1].y;
		q[1].z	= (p[1].z - p[0].z) / q[1].y;
		q[1].c	= (p[1].c - p[0].c) / q[1].y;
		q[1].d	= (p[1].d - p[0].d) / q[1].y;
	}

	if( (q[2].y	= p[2].y - p[1].y) > 0.0 )
	{
		q[2].x	= (p[2].x - p[1].x) / q[2].y;
		q[2].z	= (p[2].z - p[1].z) / q[2].y;
		q[2].c	= (p[2].c - p[1].c) / q[2].y;
		q[2].d	= (p[2].d - p[1].d) / q[2].y;
	}

	//-----------------------------------------------------
	int	ay	= (int)r.yMin;	if( ay < 0 )	ay	= 0;	if( ay < r.yMin )	ay++;
	int	by	= (int)r.yMax;	if( by >= m_Image_NY )	by	= m_Image_NY - 1;

	for(int y=ay; y<=by; y++)
	{
		if( y <= p[1].y && q[1].y > 0.0 )
		{
			_Draw_Triangle_Line(y,
				p[0].x + (y - p[0].y) * q[0].x,
				p[0].x + (y - p[0].y) * q[1].x,
				p[0].z + (y - p[0].y) * q[0].z,
				p[0].z + (y - p[0].y) * q[1].z,
				p[0].c + (y - p[0].y) * q[0].c,
				p[0].c + (y - p[0].y) * q[1].c,
				p[0].d + (y - p[0].y) * q[0].d,
				p[0].d + (y - p[0].y) * q[1].d,
				dim
			);
		}
		else if( q[2].y > 0.0 )
		{
			_Draw_Triangle_Line(y,
				p[0].x + (y - p[0].y) * q[0].x,
				p[1].x + (y - p[1].y) * q[2].x,
				p[0].z + (y - p[0].y) * q[0].z,
				p[1].z + (y - p[1].y) * q[2].z,
				p[0].c + (y - p[0].y) * q[0].c,
				p[1].c + (y - p[1].y) * q[2].c,
				p[0].d + (y - p[0].y) * q[0].d,
				p[1].d + (y - p[1].y) * q[2].d,
				dim
			);
		}
	}
}

//---------------------------------------------------------
inline void CSG_3DView_Canvas::_Draw_Triangle_Line(int y, double xa, double xb, double za, double zb, double ca, double cb, double da, double db, double dim)
{
	if( xb < xa )
	{
		double	d;

		d	= xa;	xa	= xb;	xb	= d;
		d	= za;	za	= zb;	zb	= d;
		d	= ca;	ca	= cb;	cb	= d;
		d	= da;	da	= db;	db	= d;
	}

	if( xb > xa )
	{
		double	dz	= (zb - za) / (xb - xa);
		double	dc	= (cb - ca) / (xb - xa);
		double	dd	= (db - da) / (xb - xa);

		int		ax	= (int)xa;	if( ax < 0 )	ax	= 0;	if( ax < xa )	ax++;
		int		bx	= (int)xb;	if( bx >= m_Image_NX )	bx	= m_Image_NX - 1;

		for(int x=ax; x<=bx; x++)
		{
			double	z	= za + dz * (x - xa);
			double	c	= ca + dc * (x - xa);
			double	d	= da + dd * (x - xa);

			if( m_pDrape )
			{
				if( m_pDrape->Get_Value(c, d, c, m_Drape_Mode, false, true) )
				{
					_Draw_Pixel(x, y, z, _Dim_Color((int)(0.5 + c), dim));
				}
			}
			else
			{
				_Draw_Pixel(x, y, z, _Dim_Color(Get_Color(c), dim));
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
