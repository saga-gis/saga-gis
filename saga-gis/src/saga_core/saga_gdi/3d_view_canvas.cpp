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

	static bool	bDrawing	= false;

	if( bDrawing )
	{
		return( false );
	}

	bDrawing	= true;

	_Draw_Background();

	//-------------------------------------------------
	if( m_Data_Min.x >= m_Data_Max.x
	||  m_Data_Min.y >= m_Data_Max.y
	||  m_Data_Min.z >  m_Data_Max.z )
	{
		bDrawing	= false;

		return( false );
	}

	//-------------------------------------------------
	if( !On_Before_Draw() )
	{
		bDrawing	= false;

		return( false );
	}

	//-------------------------------------------------
	m_Projector.Set_Center(
		m_Data_Min.x + 0.5 * (m_Data_Max.x - m_Data_Min.x),
		m_Data_Min.y + 0.5 * (m_Data_Max.y - m_Data_Min.y),
		m_Data_Min.z + 0.5 * (m_Data_Max.z - m_Data_Min.z)
	);

	m_Projector.Set_Scale(SG_Get_Length(m_Image_NX, m_Image_NY) / SG_Get_Length(m_Data_Max.x - m_Data_Min.x, m_Data_Max.y - m_Data_Min.y));

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

	bDrawing	= false;

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
void CSG_3DView_Canvas::Draw_Triangle(TSG_Triangle_Node p[3], bool bValueAsColor, double Light_Dec, double Light_Azi)
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

	Draw_Triangle(p, bValueAsColor, (acos(sin(s) * sin(Light_Dec) + cos(s) * cos(Light_Dec) * cos(a - Light_Azi))) / M_PI_090);
}

//---------------------------------------------------------
#define TRIANGLE_SET_POINT(dst, index)	{	int i = index;\
	dst[0]	= Point[i].x;\
	dst[1]	= Point[i].y;\
	dst[2]	= Point[i].z;\
	switch( mode )\
	{\
	default:\
		dst[3]	= Point[i].c;\
		break;\
	case 1:\
		dst[3]	= Point[i].c;\
		dst[4]	= Point[i].d;\
		break;\
	case 2:\
		dst[3]	= SG_GET_R((int)Point[i].c);\
		dst[4]	= SG_GET_G((int)Point[i].c);\
		dst[5]	= SG_GET_B((int)Point[i].c);\
		break;\
	}\
}

//---------------------------------------------------------
#define TRIANGLE_GET_GRADIENT(dst, A, B)	if( (dst[1] = B[1] - A[1]) > 0.0 ) {\
	switch( mode )\
	{\
	case 2:	dst[5] = (B[5] - A[5]) / dst[1];\
	case 1:	dst[4] = (B[4] - A[4]) / dst[1];\
	case 0:	dst[3] = (B[3] - A[3]) / dst[1];\
			dst[2] = (B[2] - A[2]) / dst[1];\
			dst[0] = (B[0] - A[0]) / dst[1];\
	}\
}

//---------------------------------------------------------
#define TRIANGLE_SET_GRADIENT(dst, P, D)	{ double dy = y - P[1];\
	switch( mode )\
	{\
	case 2:	dst[5]	= P[5] + D[5] * dy;\
	case 1:	dst[4]	= P[4] + D[4] * dy;\
	case 0:	dst[3]	= P[3] + D[3] * dy;\
			dst[2]	= P[2] + D[2] * dy;\
			dst[0]	= P[0] + D[0] * dy;\
	}\
}

//---------------------------------------------------------
void CSG_3DView_Canvas::Draw_Triangle(TSG_Triangle_Node Point[3], bool bValueAsColor, double dim)
{
	//-----------------------------------------------------
	if( Point[0].z < 0.0 || Point[1].z < 0.0 || Point[2].z < 0.0 )
	{
		return;	// completely in front of projection plane
	}

	CSG_Rect	r(
		Point[0].x, Point[0].y,
		Point[1].x, Point[1].y
	);

	r.Union(CSG_Point(
		Point[2].x, Point[2].y
	));

	if( r.Get_XMax() < 0.0 || r.Get_XMin() >= m_Image_NX
	||	r.Get_YMax() < 0.0 || r.Get_YMin() >= m_Image_NY )
	{
		return;	// completely off screen
	}

	if( r.Get_XRange() <= 0.0 || r.Get_YRange() <= 0.0 )
	{
		return;	// has no area (... should draw a point ?!)
	}

	//-----------------------------------------------------
	int	mode	= m_pDrape ? 1 : bValueAsColor ? 2 : 0;

	double	p[3][6], d[3][6], a[6], b[6];

	if( Point[0].y < Point[1].y && Point[0].y < Point[2].y )
	{
		TRIANGLE_SET_POINT(p[0], 0);								// top point
		TRIANGLE_SET_POINT(p[1], Point[1].y < Point[2].y ? 1 : 2);	// middle point
		TRIANGLE_SET_POINT(p[2], Point[1].y < Point[2].y ? 2 : 1);	// bottom point
	}
	else if( Point[1].y < Point[0].y && Point[1].y < Point[2].y )
	{
		TRIANGLE_SET_POINT(p[0], 1);								// top point
		TRIANGLE_SET_POINT(p[1], Point[0].y < Point[2].y ? 0 : 2);	// middle point
		TRIANGLE_SET_POINT(p[2], Point[0].y < Point[2].y ? 2 : 0);	// bottom point
	}
	else // if( Point[2].y < Point[0].y && Point[2].y < Point[1].y )
	{
		TRIANGLE_SET_POINT(p[0], 2);								// top point
		TRIANGLE_SET_POINT(p[1], Point[0].y < Point[1].y ? 0 : 1);	// middle point
		TRIANGLE_SET_POINT(p[2], Point[0].y < Point[1].y ? 1 : 0);	// bottom point
	}

	TRIANGLE_GET_GRADIENT(d[0], p[0], p[2]);	// from top to bottom point
	TRIANGLE_GET_GRADIENT(d[1], p[0], p[1]);	// from top to midlle point
	TRIANGLE_GET_GRADIENT(d[2], p[1], p[2]);	// from middle to bottom point

	int	ay	= (int)r.Get_YMin(); if( ay < 0           ) ay	= 0; if( ay < r.Get_YMin() ) ay++;
	int	by	= (int)r.Get_YMax(); if( by >= m_Image_NY ) by	= m_Image_NY - 1;

	//-----------------------------------------------------
	for(int y=ay; y<=by; y++)
	{
		if( y <= p[1][1] && d[1][1] > 0.0 )
		{
			TRIANGLE_SET_GRADIENT(a, p[0], d[0]);	//	a	= p[0] + d[0] * (y - p[0][1]);	// using CSG_Vector is, unluckily, significantly slower!!
			TRIANGLE_SET_GRADIENT(b, p[0], d[1]);

			if( a[0] < b[0] )
			{
				_Draw_Triangle_Line(y, a, b, dim, mode);
			}
			else
			{
				_Draw_Triangle_Line(y, b, a, dim, mode);
			}
		}
		else if( d[2][1] > 0.0 )
		{
			TRIANGLE_SET_GRADIENT(a, p[0], d[0]);
			TRIANGLE_SET_GRADIENT(b, p[1], d[2]);

			if( a[0] < b[0] )
			{
				_Draw_Triangle_Line(y, a, b, dim, mode);
			}
			else
			{
				_Draw_Triangle_Line(y, b, a, dim, mode);
			}
		}
	}
}

//---------------------------------------------------------
inline void CSG_3DView_Canvas::_Draw_Triangle_Line(int y, double a[], double b[], double dim, int mode)
{
	if( a[0] == b[0] )
	{
		if( a[2] < b[2] )
		{
			_Draw_Pixel((int)a[0], y, a[2], _Dim_Color(Get_Color(a[3]), dim));
		}
		else
		{
			_Draw_Pixel((int)b[0], y, b[2], _Dim_Color(Get_Color(b[3]), dim));
		}

		return;
	}

	double	d[6], dx = b[0] - a[0];

	switch( mode )
	{
	case 2:	d[5]	= (b[5] - a[5]) / dx;
	case 1:	d[4]	= (b[4] - a[4]) / dx;
	}

	d[3]	= (b[3] - a[3]) / dx;
	d[2]	= (b[2] - a[2]) / dx;

	int	ax	= (int)a[0]; if( ax <  0          )	ax	= 0;
	int	bx	= (int)b[0]; if( bx >= m_Image_NX )	bx	= m_Image_NX - 1;

	dx	= ax - a[0];

	for(int x=ax; x<=bx; x++, dx++)
	{
		double	z	= a[2] + dx * d[2];

		switch( mode )
		{
		default:
			{
				_Draw_Pixel(x, y, z, _Dim_Color(Get_Color(a[3] + dx * d[3]), dim));
			}
			break;

		case 1:
			{
				double	Value;

				if( m_pDrape->Get_Value(a[3] + dx * d[3], a[4] + dx * d[4], Value, m_Drape_Mode, false, true) )
				{
					_Draw_Pixel(x, y, z, _Dim_Color((int)Value, dim));
				}
			}
			break;

		case 2:
			{
				_Draw_Pixel(x, y, z, _Dim_Color(SG_GET_RGB(
					a[3] + dx * d[3],
					a[4] + dx * d[4],
					a[5] + dx * d[5])
				, dim));
			}
			break;
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
