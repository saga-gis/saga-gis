
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
#include "3d_view_tools.h"

#include "sgdi_helper.h"

#include <wx/dcmemory.h>


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
	m_pDrape     = NULL;
	m_Image_pRGB = NULL;

	m_bgColor    = SG_COLOR_WHITE;
	m_bBox       = true;
	m_bLabels    = false;
	m_bNorth     = false;
	m_BoxBuffer  = 0.01;
	m_bStereo    = false;
	m_dStereo    = 2.;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_3DView_Canvas::Set_Image(BYTE *pRGB, int NX, int NY)
{
	m_Image_pRGB = pRGB;

	m_Image_NX   = NX;
	m_Image_NY   = NY;

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
		m_Image_zMax.Assign(999999.);

		m_Color_Mode	= COLOR_MODE_RGB;

		On_Draw();

		_Draw_Box();
		_Draw_Labels();
	}

	//-----------------------------------------------------
	else
	{
		double	dRotate	= M_DEG_TO_RAD * 0.5 * m_dStereo;
		double	yRotate	= m_Projector.Get_yRotation();
		double	dShift	= -0.01 * m_Image_NX;
		double	xShift	= m_Projector.Get_xShift();

		//-------------------------------------------------
		m_Image_zMax.Assign(999999.);

		m_Projector.Set_xShift   (xShift  - dShift );
		m_Projector.Set_yRotation(yRotate - dRotate);

		m_Color_Mode	= COLOR_MODE_RED;

		On_Draw();

		_Draw_Box();
		_Draw_Labels();

		//-------------------------------------------------
		m_Image_zMax.Assign(999999.);

		m_Projector.Set_xShift   (xShift  + dShift );
		m_Projector.Set_yRotation(yRotate + dRotate);

		m_Color_Mode	= COLOR_MODE_CYAN;

		On_Draw();

		_Draw_Box();
		_Draw_Labels();

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
		r	= g	= b	= (int)((SG_GET_R(m_bgColor) + SG_GET_G(m_bgColor) + SG_GET_B(m_bgColor)) / 3.);
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
void CSG_3DView_Canvas::_Draw_Get_Box(TSG_Point_Z Box[8], bool bProjected)
{
	TSG_Point_Z	Buffer;

	Buffer.x = m_BoxBuffer * (m_Data_Max.x - m_Data_Min.x);
	Buffer.y = m_BoxBuffer * (m_Data_Max.y - m_Data_Min.y);
	Buffer.z = m_BoxBuffer * (m_Data_Max.z - m_Data_Min.z);

	Box[0].x = Box[4].x = m_Data_Min.x - Buffer.x; Box[0].y = Box[4].y = m_Data_Min.y - Buffer.y;
	Box[1].x = Box[5].x = m_Data_Max.x + Buffer.x; Box[1].y = Box[5].y = m_Data_Min.y - Buffer.y;
	Box[2].x = Box[6].x = m_Data_Max.x + Buffer.x; Box[2].y = Box[6].y = m_Data_Max.y + Buffer.y;
	Box[3].x = Box[7].x = m_Data_Min.x - Buffer.x; Box[3].y = Box[7].y = m_Data_Max.y + Buffer.y;
	Box[0].z = Box[1].z = Box[2].z = Box[3].z = m_Data_Min.z - Buffer.z;
	Box[4].z = Box[5].z = Box[6].z = Box[7].z = m_Data_Max.z + Buffer.z;

	if( bProjected )
	{
		for(int i=0; i<8; i++)
		{
			m_Projector.Get_Projection(Box[i]);
		}
	}
}

//---------------------------------------------------------
void CSG_3DView_Canvas::_Draw_Box(void)
{
	if( !m_bBox )
	{
		return;
	}

	int Color = SG_GET_RGB(SG_GET_R(m_bgColor) + 128, SG_GET_G(m_bgColor) + 128, SG_GET_B(m_bgColor) + 128);

	TSG_Point_Z	Box[8]; _Draw_Get_Box(Box, true);

	for(int i=0; i<8; i+=4)
	{
		Draw_Line(Box[i + 0], Box[i + 1], Color);
		Draw_Line(Box[i + 1], Box[i + 2], Color);
		Draw_Line(Box[i + 2], Box[i + 3], Color);
		Draw_Line(Box[i + 3], Box[i + 0], Color);
	}

	Draw_Line(Box[0], Box[4], Color);
	Draw_Line(Box[1], Box[5], Color);
	Draw_Line(Box[2], Box[6], Color);
	Draw_Line(Box[3], Box[7], Color);
}

//---------------------------------------------------------
void CSG_3DView_Canvas::_Draw_Labels(void)
{
	if( !m_bLabels )
	{
		return;
	}

	TSG_Point_Z	Box[8]; _Draw_Get_Box(Box, false);

	double Scale = (Box[1].x - Box[0].x) / 1000.;

	_Draw_Label(Scale, Box[0].x, Box[1].x, Box[0],   0,   0,   0);
	_Draw_Label(Scale, Box[1].y, Box[2].y, Box[1],  90,   0,   0);
	_Draw_Label(Scale, Box[2].x, Box[3].x, Box[2], 180,   0,   0);
	_Draw_Label(Scale, Box[3].y, Box[0].y, Box[3], 270,   0,   0);

//	Scale = (Box[4].z - Box[0].z) / 1000.;
//	_Draw_Label(Scale, Box[0].z, Box[4].z, Box[0],   0, -90,   0);
}

//---------------------------------------------------------
void CSG_3DView_Canvas::_Draw_Label(double Scale, double valMin, double valMax, const TSG_Point_Z &Point, double Rx, double Ry, double Rz)
{
	bool bAscending = valMax > valMin; if( !bAscending ) { double val = valMin; valMin = valMax; valMax = val; }

	wxSize Size((valMax - valMin) / Scale, 50);//400 / Scale);

	if( Size.GetWidth() < 1 || Size.GetHeight() < 1 )
	{
		return;
	}

	wxBitmap Bitmap(Size, 32); wxMemoryDC dc(Bitmap);

	wxColour FGColor(SG_GET_R(m_bgColor) + 128, SG_GET_G(m_bgColor) + 128, SG_GET_B(m_bgColor) + 128);
	wxColour BGColor(SG_GET_R(m_bgColor)      , SG_GET_G(m_bgColor)      , SG_GET_B(m_bgColor)      );

	dc.SetBackground(BGColor); dc.Clear(); dc.SetPen(FGColor); dc.SetTextForeground(FGColor);
	if( !m_bBox ) { dc.DrawLine(0, 0, Size.GetWidth(), 0); }

	Draw_Scale(dc, Size, valMin, valMax, true, bAscending, true);

	CSG_Vector P(3); P[0] = Point.x; P[1] = Point.y; P[2] = Point.z;
	CSG_Matrix R = SG_Matrix_Get_Rotation(Rx, Ry, Rz, true); R *= Scale;

	wxImage Image(Bitmap.ConvertToImage()); _Draw_Image(Image, P, R, m_bgColor);
}

//---------------------------------------------------------
void CSG_3DView_Canvas::_Draw_Image(wxImage &Image, const CSG_Vector &Move, const CSG_Matrix &Rotate, int BGColor)
{
	#define GET_NODE(xImage, yImage, node, color) {\
		CSG_Vector v(3); v[0] = xImage; v[1] = -yImage; v[2] = 0; v = Move + Rotate * v;\
		TSG_Point_Z	point; point.x = v[0]; point.y = v[1]; point.z = v[2];\
		m_Projector.Get_Projection(point);\
		node.x = point.x; node.y = point.y; node.z = point.z; node.c = color;\
	}

	//-----------------------------------------------------
	CSG_Grid *pDrape = m_pDrape; m_pDrape = NULL;

	//-----------------------------------------------------
	#ifndef _DEBUG
	#pragma omp parallel for
	#endif
	for(int y1=1; y1<Image.GetHeight(); y1++)
	{
		int y0 = y1 - 1;

		for(int x0=0, x1=1; x1<Image.GetWidth(); x0++, x1++)
		{
			int c[4];

			c[0] = SG_GET_RGB(Image.GetRed(x0, y0), Image.GetGreen(x0, y0), Image.GetBlue(x0, y0));
			c[1] = SG_GET_RGB(Image.GetRed(x1, y0), Image.GetGreen(x1, y0), Image.GetBlue(x1, y0));
			c[2] = SG_GET_RGB(Image.GetRed(x1, y1), Image.GetGreen(x1, y1), Image.GetBlue(x1, y1));
			c[3] = SG_GET_RGB(Image.GetRed(x0, y1), Image.GetGreen(x0, y1), Image.GetBlue(x0, y1));

			int n = (c[0] != BGColor ? 1 : 0)
				  +	(c[1] != BGColor ? 1 : 0)
				  + (c[2] != BGColor ? 1 : 0)
				  + (c[3] != BGColor ? 1 : 0);

			if( n > 0 )
			{
				TSG_Triangle_Node p[4], Triangle[3];

				GET_NODE(x0, y0, p[0], c[0]);
				GET_NODE(x1, y0, p[1], c[1]);
				GET_NODE(x1, y1, p[2], c[2]);
				GET_NODE(x0, y1, p[3], c[3]);

				int r = (SG_GET_R(c[0]) + SG_GET_R(c[1]) + SG_GET_R(c[2]) + SG_GET_R(c[3])) / 4;
				int g = (SG_GET_G(c[0]) + SG_GET_G(c[1]) + SG_GET_G(c[2]) + SG_GET_G(c[3])) / 4;
				int b = (SG_GET_B(c[0]) + SG_GET_B(c[1]) + SG_GET_B(c[2]) + SG_GET_B(c[3])) / 4;

				Triangle[2].x = (p[0].x + p[1].x + p[2].x + p[3].x) / 4.;
				Triangle[2].y = (p[0].y + p[1].y + p[2].y + p[3].y) / 4.;
				Triangle[2].z = (p[0].z + p[1].z + p[2].z + p[3].z) / 4.;
				Triangle[2].c = SG_GET_RGB(
					r > 255 ? 255 : r < 0 ? 0 : r,
					g > 255 ? 255 : g < 0 ? 0 : g,
					b > 255 ? 255 : b < 0 ? 0 : b
				);

				Triangle[0] = p[0];
				Triangle[1] = p[1]; Draw_Triangle(Triangle, true);
				Triangle[0] = p[2]; Draw_Triangle(Triangle, true);
				Triangle[1] = p[3]; Draw_Triangle(Triangle, true);
				Triangle[0] = p[0]; Draw_Triangle(Triangle, true);
			}
		}
	}

	m_pDrape = pDrape;
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
int CSG_3DView_Canvas::Dim_Color(int Color, double Dim)
{
	if( Dim > 0. )
	{
		if( Dim != 1. )
		{
			int r = SG_GET_R(m_bgColor) + (int)(Dim * (SG_GET_R(Color) - SG_GET_R(m_bgColor)));
			int g = SG_GET_G(m_bgColor) + (int)(Dim * (SG_GET_G(Color) - SG_GET_G(m_bgColor)));
			int b = SG_GET_B(m_bgColor) + (int)(Dim * (SG_GET_B(Color) - SG_GET_B(m_bgColor)));

			Color = SG_GET_RGB(r > 255 ? 255 : r, g > 255 ? 255 : g, b > 255 ? 255 : b);
		}

		return( Color );
	}

	return( m_bgColor );
}

//---------------------------------------------------------
int CSG_3DView_Canvas::_Dim_Color(int Color, double Dim)
{
	if( Dim > 0. )
	{
		if( Dim != 1. )
		{
			int r = (int)(Dim * SG_GET_R(Color));
			int g = (int)(Dim * SG_GET_G(Color));
			int b = (int)(Dim * SG_GET_B(Color));

			Color = SG_GET_RGB(r > 255 ? 255 : r, g > 255 ? 255 : g, b > 255 ? 255 : b);
		}

		return( Color );
	}

	return( 0 );
}

//---------------------------------------------------------
inline void CSG_3DView_Canvas::_Draw_Pixel(int x, int y, double z, int color)
{
	if( x >= 0 && x < m_Image_NX && y >= 0 && y < m_Image_NY && z < m_Image_zMax[y][x] )
	{
		BYTE *RGB = m_Image_pRGB + 3 * (y * m_Image_NX + x);

		switch( m_Color_Mode )
		{
		case COLOR_MODE_RGB:
			RGB[0] = SG_GET_R(color);
			RGB[1] = SG_GET_G(color);
			RGB[2] = SG_GET_B(color);
			break;

		case COLOR_MODE_RED:
			RGB[0] = (SG_GET_R(color) + SG_GET_G(color) + SG_GET_B(color)) / 3;
			break;

		case COLOR_MODE_GREEN:
			RGB[1] = (SG_GET_R(color) + SG_GET_G(color) + SG_GET_B(color)) / 3;
			break;

		case COLOR_MODE_BLUE:
			RGB[2] = (SG_GET_R(color) + SG_GET_G(color) + SG_GET_B(color)) / 3;
			break;

		case COLOR_MODE_CYAN:
			RGB[1] = 
			RGB[2] = (SG_GET_R(color) + SG_GET_G(color) + SG_GET_B(color)) / 3;
			break;
		}

		m_Image_zMax[y][x] = z;
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_3DView_Canvas::Draw_Point(int x, int y, double z, int Color, int Size)
{
	if( z > 0. )
	{
		_Draw_Pixel(x, y, z, Color);

		if( Size > 0 && Size < 50 )
		{
			for(int iy=1; iy<=Size; iy++) for(int ix=0; ix<=Size; ix++)
			{
				if( ix*ix + iy*iy <= Size*Size )
				{
					_Draw_Pixel(x + ix, y + iy, z, Color);
					_Draw_Pixel(x + iy, y - ix, z, Color);
					_Draw_Pixel(x - ix, y - iy, z, Color);
					_Draw_Pixel(x - iy, y + ix, z, Color);
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
		return; // completely out of area
	}

	if( bz < 0. && az < 0. )
	{
		return; // completely in front of screen
	}

	double n, dx = bx - ax, dy = by - ay, dz = bz - az;

	if( fabs(dx) > fabs(dy) && fabs(dx) > 0. )
	{
		n = fabs(dx); dx = dx < 0. ? -1. : 1.; dy /= n; dz /= n;
	}
	else if( fabs(dy) > 0. )
	{
		n = fabs(dy); dy = dy < 0. ? -1. : 1.; dx /= n; dz /= n;
	}
	else
	{
		_Draw_Pixel((int)ax, (int)ay, az, aColor);
		_Draw_Pixel((int)bx, (int)by, bz, bColor);

		return;
	}

	//-----------------------------------------------------
	CSG_Colors Colors(2); Colors[0] = aColor; Colors[1] = bColor;

	for(double i=0.; i<=n; i++, ax+=dx, ay+=dy, az+=dz)
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
		return; // completely out of area
	}

	if( bz < 0. || az < 0. )
	{
		return; // completely in front of screen
	}

	double n, dx = bx - ax, dy = by - ay, dz = bz - az;

	if( fabs(dx) > fabs(dy) && fabs(dx) > 0. )
	{
		n = fabs(dx); dx = dx < 0. ? -1. : 1.; dy /= n; dz /= n;
	}
	else if( fabs(dy) > 0. )
	{
		n = fabs(dy); dy = dy < 0. ? -1. : 1.; dx /= n; dz /= n;
	}
	else
	{
		_Draw_Pixel((int)ax, (int)ay, az, Color);

		return;
	}

	//-----------------------------------------------------
	for(double i=0.; i<=n; i++, ax+=dx, ay+=dy, az+=dz)
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
	double s, a, C = p[0].x * (p[1].y - p[2].y) + p[1].x * (p[2].y - p[0].y) + p[2].x * (p[0].y - p[1].y);

	if( C != 0. )
	{
		double A = p[0].z * (p[1].x - p[2].x) + p[1].z * (p[2].x - p[0].x) + p[2].z * (p[0].x - p[1].x);
		double B = p[0].y * (p[1].z - p[2].z) + p[1].y * (p[2].z - p[0].z) + p[2].y * (p[0].z - p[1].z);

		A = -A / C;
		B = -B / C;

		s = M_PI_090 - atan(sqrt(A*A + B*B));
		a = A != 0. ? M_PI_180 + atan2(B, A) : B > 0. ? M_PI_270 : (B < 0. ? M_PI_090 : -1.);
	}
	else
	{
		s = 0.;
		a = 0.;
	}

	Draw_Triangle(p, bValueAsColor, (acos(sin(s) * sin(Light_Dec) + cos(s) * cos(Light_Dec) * cos(a - Light_Azi))) / M_PI_090);
}

//---------------------------------------------------------
#define TRIANGLE_SET_POINT(P, i) {\
	P[0] = Point[i].x;\
	P[1] = Point[i].y;\
	P[2] = Point[i].z;\
	switch( Mode )\
	{\
	case 0:\
		P[3] = Point[i].c;\
		break;\
	case 1:\
		P[3] = Point[i].c;\
		P[4] = Point[i].d;\
		break;\
	case 2:\
		P[3] = SG_GET_R((int)Point[i].c);\
		P[4] = SG_GET_G((int)Point[i].c);\
		P[5] = SG_GET_B((int)Point[i].c);\
		break;\
	}\
}

//---------------------------------------------------------
#define TRIANGLE_GET_GRADIENT(G, A, B) if( (G[1] = B[1] - A[1]) > 0. ) {\
	switch( Mode )\
	{\
	case 2:\
		G[5] = (B[5] - A[5]) / G[1];\
	case 1:\
		G[4] = (B[4] - A[4]) / G[1];\
	case 0:\
		G[3] = (B[3] - A[3]) / G[1];\
		G[2] = (B[2] - A[2]) / G[1];\
		G[0] = (B[0] - A[0]) / G[1];\
	}\
}

//---------------------------------------------------------
#define TRIANGLE_SET_GRADIENT(G, P, D) { double dy = y - P[1];\
	switch( Mode )\
	{\
	case 2:\
		G[5] = P[5] + D[5] * dy;\
	case 1:\
		G[4] = P[4] + D[4] * dy;\
	case 0:\
		G[3] = P[3] + D[3] * dy;\
		G[2] = P[2] + D[2] * dy;\
		G[0] = P[0] + D[0] * dy;\
	}\
}

//---------------------------------------------------------
void CSG_3DView_Canvas::Draw_Triangle(TSG_Triangle_Node Point[3], bool bValueAsColor, double Dim)
{
	if( Point[0].z < 0. || Point[1].z < 0. || Point[2].z < 0. )
	{
		return;	// completely in front of projection plane
	}

	CSG_Rect r(
		Point[0].x, Point[0].y,
		Point[1].x, Point[1].y
	);

	r.Union(CSG_Point(
		Point[2].x, Point[2].y
	));

	if( r.Get_XMax() < 0. || r.Get_XMin() >= m_Image_NX
	||	r.Get_YMax() < 0. || r.Get_YMin() >= m_Image_NY )
	{
		return;	// completely off screen
	}

	if( r.Get_XRange() <= 0. || r.Get_YRange() <= 0. )
	{
		return;	// has no area (... should draw a point ?!)
	}

	//-----------------------------------------------------
	int	Mode	= m_pDrape ? 1 : bValueAsColor ? 2 : 0;

	double p[3][6], d[3][6], a[6], b[6];

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

	int ay = (int)(r.Get_YMin()); if( ay < 0           ) ay = 0; if( ay < r.Get_YMin() ) ay++;
	int by = (int)(r.Get_YMax()); if( by >= m_Image_NY ) by = m_Image_NY - 1;

	//-----------------------------------------------------
	for(int y=ay; y<=by; y++)
	{
		if( y <= p[1][1] && d[1][1] > 0. )
		{
			TRIANGLE_SET_GRADIENT(a, p[0], d[0]);	//	a	= p[0] + d[0] * (y - p[0][1]);	// using CSG_Vector is, unluckily, significantly slower!!
			TRIANGLE_SET_GRADIENT(b, p[0], d[1]);

			if( a[0] < b[0] )
			{
				_Draw_Triangle_Line(y, a, b, Dim, Mode);
			}
			else
			{
				_Draw_Triangle_Line(y, b, a, Dim, Mode);
			}
		}
		else if( d[2][1] > 0. )
		{
			TRIANGLE_SET_GRADIENT(a, p[0], d[0]);
			TRIANGLE_SET_GRADIENT(b, p[1], d[2]);

			if( a[0] < b[0] )
			{
				_Draw_Triangle_Line(y, a, b, Dim, Mode);
			}
			else
			{
				_Draw_Triangle_Line(y, b, a, Dim, Mode);
			}
		}
	}
}

//---------------------------------------------------------
inline void CSG_3DView_Canvas::_Draw_Triangle_Line(int y, double a[], double b[], double Dim, int mode)
{
	if( a[0] == b[0] )
	{
		if( a[2] < b[2] )
		{
			_Draw_Pixel((int)a[0], y, a[2], _Dim_Color(Get_Color(a[3]), Dim));
		}
		else
		{
			_Draw_Pixel((int)b[0], y, b[2], _Dim_Color(Get_Color(b[3]), Dim));
		}

		return;
	}

	//-----------------------------------------------------
	double d[6], dx; int ax, bx;

	ax   = (int)a[0]; if( ax <  0          ) ax = 0;
	bx   = (int)b[0]; if( bx >= m_Image_NX ) bx = m_Image_NX - 1;
	dx   = ax - a[0];
	d[3] = (b[3] - a[3]) / (b[0] - a[0]);
	d[2] = (b[2] - a[2]) / (b[0] - a[0]);

	//-----------------------------------------------------
	switch( mode )
	{
	default: {
		for(int x=ax; x<=bx; x++, dx++)
		{
			_Draw_Pixel(x, y, a[2] + dx * d[2], _Dim_Color(Get_Color(a[3] + dx * d[3]), Dim));
		}
		break; }

	case 1: {
		double Value; d[4] = (b[4] - a[4]) / (b[0] - a[0]);

		for(int x=ax; x<=bx; x++, dx++)
		{
			if( m_pDrape->Get_Value(a[3] + dx * d[3], a[4] + dx * d[4], Value, m_Drape_Mode, false, true) )
			{
				_Draw_Pixel(x, y, a[2] + dx * d[2], _Dim_Color((int)Value, Dim));
			}
		}
		break; }

	case 2: {
		CSG_Colors Color(2); Color.Set_Ramp(
			SG_GET_RGB((int)a[3], (int)a[4], (int)a[5]),
			SG_GET_RGB((int)b[3], (int)b[4], (int)b[5])
		);

		for(int x=ax; x<=bx; x++, dx++)
		{
			_Draw_Pixel(x, y, a[2] + dx * d[2], _Dim_Color(Color.Get_Interpolated(dx), Dim));
		}
		break; }
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
