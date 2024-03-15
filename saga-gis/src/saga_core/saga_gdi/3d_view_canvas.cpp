
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
//                                                       //
//                                                       //
//                                                       //
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
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_3DView_Canvas::CSG_3DView_Canvas(void)
{
	m_pDrape      = NULL;
	m_Image_pRGB  = NULL;

	m_bgColor     = SG_COLOR_WHITE;
	m_bBox        = true;
	m_BoxBuffer   = 0.01;
	m_Stereo      = 0;
	m_dStereo     = 1.;
	m_North       = 1;
	m_North_Size  = 15.;
	m_Labels      = 0;
	m_Label_Res   = 50;
	m_Label_Scale = 1.;
	m_Label_zType = SG_DATATYPE_Undefined; // number
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_3DView_Canvas::Set_Image(wxImage &Image)
{
	if( Image.IsOk() && Image.GetWidth() > 0 && Image.GetHeight() > 0 )
	{
		m_Image_pRGB = Image.GetData  (); m_Image_pTwin = NULL;
		m_Image_NX   = Image.GetWidth ();
		m_Image_NY   = Image.GetHeight();
		m_Image_zMax.Create   (m_Image_NX, m_Image_NY);
		m_Projector.Set_Screen(m_Image_NX, m_Image_NY);

		return( true );
	}

	m_Image_pRGB = NULL; m_Image_pTwin = NULL;
	m_Image_NX   = 0;
	m_Image_NY   = 0;
	m_Image_zMax.Destroy();

	return( false );
}

//---------------------------------------------------------
bool CSG_3DView_Canvas::Set_Image_Twin(wxImage &Image)
{
	if( Image.IsOk() && Image.GetWidth() == m_Image_NX && Image.GetHeight() == m_Image_NY )
	{
		m_Image_pTwin = Image.GetData();

		return( true );
	}

	m_Image_pTwin = NULL;

	return( false );
}

//---------------------------------------------------------
bool CSG_3DView_Canvas::Set_Drape(CSG_Grid *pDrape)
{
	m_pDrape = pDrape;

	return( m_pDrape != NULL );
}

//---------------------------------------------------------
bool CSG_3DView_Canvas::Draw(void)
{
	if( !m_Image_pRGB || m_Image_NX < 1 || m_Image_NY < 1 )
	{
		return( false );
	}

	static bool bDrawing = false;

	if( bDrawing )
	{
		return( false );
	}

	bDrawing = true;

	_Draw_Background();

	//-----------------------------------------------------
	if( m_Data_Min.x >= m_Data_Max.x
	||  m_Data_Min.y >= m_Data_Max.y
	||  m_Data_Min.z >  m_Data_Max.z )
	{
		bDrawing = false;

		return( false );
	}

	//-----------------------------------------------------
	if( !On_Before_Draw() )
	{
		bDrawing = false;

		return( false );
	}

	//-----------------------------------------------------
	m_Projector.Set_Center( // rotation center set to data's center
		m_Data_Min.x + (m_Data_Max.x - m_Data_Min.x) / 2.,
		m_Data_Min.y + (m_Data_Max.y - m_Data_Min.y) / 2.,
		m_Data_Min.z + (m_Data_Max.z - m_Data_Min.z) / 2.
	);

	m_Projector.Set_Scale(SG_Get_Length(m_Data_Max.x - m_Data_Min.x, m_Data_Max.y - m_Data_Min.y));

	int Front = _Draw_Get_Box_Front();

	//-----------------------------------------------------
	if( m_Stereo == 0 || (m_Stereo == 2 && m_Image_pTwin == NULL) )
	{
		m_Color_Mode = COLOR_MODE_RGB;

		m_Image_zMax.Assign(999999.); On_Draw(); _Draw_Box(); _Draw_Labels(Front); _Draw_North();
	}

	//-----------------------------------------------------
	else if( m_Stereo == 1 )
	{
		double rx = m_Projector.Get_xRotation();
		double ry = m_Projector.Get_yRotation(), dy = cos(rx) * m_dStereo * M_DEG_TO_RAD / 2.;
		double rz = m_Projector.Get_zRotation(), dz = sin(rx) * m_dStereo * M_DEG_TO_RAD / 2.;

		//-------------------------------------------------
		m_Projector.Set_yRotation(ry - dy);
		m_Projector.Set_zRotation(rz - dz);

		m_Color_Mode = COLOR_MODE_RED;

		m_Image_zMax.Assign(999999.); On_Draw(); _Draw_Box(); _Draw_Labels(Front); _Draw_North();

		//-------------------------------------------------
		m_Projector.Set_yRotation(ry + dy);
		m_Projector.Set_zRotation(rz + dz);

		m_Color_Mode = COLOR_MODE_CYAN;

		m_Image_zMax.Assign(999999.); On_Draw(); _Draw_Box(); _Draw_Labels(Front); _Draw_North();

		//-------------------------------------------------
		m_Projector.Set_yRotation(ry);
		m_Projector.Set_zRotation(rz);
	}

	//-----------------------------------------------------
	else // if( m_Image_pTwin )
	{
		m_Color_Mode = COLOR_MODE_RGB;

		double rx = m_Projector.Get_xRotation();
		double ry = m_Projector.Get_yRotation(), dy = cos(rx) * m_dStereo * M_DEG_TO_RAD / 2.;
		double rz = m_Projector.Get_zRotation(), dz = sin(rx) * m_dStereo * M_DEG_TO_RAD / 2.;

		//-------------------------------------------------
		m_Projector.Set_yRotation(ry - dy);
		m_Projector.Set_zRotation(rz - dz);

		m_Image_zMax.Assign(999999.); On_Draw(); _Draw_Box(); _Draw_Labels(Front); _Draw_North();

		//-------------------------------------------------
		m_Projector.Set_yRotation(ry + dy);
		m_Projector.Set_zRotation(rz + dz);

		BYTE *pRGB = m_Image_pRGB; m_Image_pRGB = m_Image_pTwin; _Draw_Background();
		m_Image_zMax.Assign(999999.); On_Draw(); _Draw_Box(); _Draw_Labels(Front); _Draw_North();
		m_Image_pRGB = pRGB;

		//-------------------------------------------------
		m_Projector.Set_yRotation(ry);
		m_Projector.Set_zRotation(rz);
	}

	bDrawing = false;

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_3DView_Canvas::_Draw_Background(void)
{
	BYTE r, g, b;

	if( m_Stereo == 1 ) // anaglyph => greyscale
	{
		r = g = b = (int)((SG_GET_R(m_bgColor) + SG_GET_G(m_bgColor) + SG_GET_B(m_bgColor)) / 3.);
	}
	else
	{
		r = SG_GET_R(m_bgColor);
		g = SG_GET_G(m_bgColor);
		b = SG_GET_B(m_bgColor);
	}

	#pragma omp parallel for
	for(int y=0; y<m_Image_NY; y++)
	{
		BYTE *pRGB = m_Image_pRGB + y * 3 * m_Image_NX;

		for(int x=0; x<m_Image_NX; x++)
		{
			*pRGB = r; pRGB++;
			*pRGB = g; pRGB++;
			*pRGB = b; pRGB++;
		}
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_3DView_Canvas::_Draw_Get_Box(TSG_Point_3D Box[8], bool bProjected)
{
	TSG_Point_3D	Buffer;

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
int CSG_3DView_Canvas::_Draw_Get_Box_Front(void)
{
	int Front = 0; TSG_Point_3D b[8]; _Draw_Get_Box(b, true);

	for(int i=1; i<4; i++)
	{
		if( b[i].z < b[Front].z )
		{
			Front = i;
		}
	}

	return( Front );
}

//---------------------------------------------------------
void CSG_3DView_Canvas::_Draw_Box(void)
{
	if( !m_bBox )
	{
		return;
	}

	int Color = SG_GET_RGB(SG_GET_R(m_bgColor) + 128, SG_GET_G(m_bgColor) + 128, SG_GET_B(m_bgColor) + 128);

	TSG_Point_3D	Box[8]; _Draw_Get_Box(Box, true);

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


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_3DView_Canvas::_Draw_North(void)
{
	if( !m_North )
	{
		return;
	}

	const double Arrow[9][2] = { { 0., 0. }, { 0., 1. }, { 0., -0.5 }, { -0.5, -1. }, { 0.5, -1. }, { -1, -1. }, { 1, -1. }, { 1, 1. }, { -1, 1. } };

	TSG_Point_3D A[9]; CSG_Rect r; double Scale = M_GET_MIN(m_Data_Max.x - m_Data_Min.x, m_Data_Max.y - m_Data_Min.y) / 2.;

	for(int i=0; i<9; i++)
	{
		A[i].x = m_Projector.Get_xCenter() + Arrow[i][0] * Scale;
		A[i].y = m_Projector.Get_yCenter() + Arrow[i][1] * Scale;
		A[i].z = m_Data_Min.z;

		m_Projector.Get_Projection(A[i]);

		if( i == 2 )
		{
			r.Assign(A[1].x, A[1].y, A[2].x, A[2].y);
		}
		else if( i > 2 )
		{
			r.Union(CSG_Point(A[i].x, A[i].y));
		}
	}

	double Size = m_North_Size * M_GET_MIN(m_Image_NX, m_Image_NY) / 100.;

	A[0].z = Size / r.Get_Diameter();

	for(int i=1; i<9; i++)
	{
		A[i].x = (A[i].x - A[0].x) * A[0].z +              0.6 * Size;
		A[i].y = (A[i].y - A[0].y) * A[0].z + m_Image_NY - 0.6 * Size;
		A[i].z = 0.;
	}

	Draw_Line(A[1], A[2], SG_COLOR_BLACK);
	Draw_Line(A[1], A[3], SG_COLOR_BLACK);
	Draw_Line(A[1], A[4], SG_COLOR_BLACK);
	Draw_Line(A[2], A[3], SG_COLOR_BLACK);
	Draw_Line(A[2], A[4], SG_COLOR_BLACK);

	if( m_North == 1 )
	{
		int Color = SG_GET_RGB(SG_GET_R(m_bgColor) + 128, SG_GET_G(m_bgColor) + 128, SG_GET_B(m_bgColor) + 128);

		Draw_Line(A[5], A[6], Color);
		Draw_Line(A[6], A[7], Color);
		Draw_Line(A[7], A[8], Color);
		Draw_Line(A[8], A[5], Color);
	}

	TSG_Triangle_Node p[3]; CSG_Grid *pDrape = m_pDrape; m_pDrape = NULL;

	#define SET_NODE(n, i, color) { p[n].x = A[i].x; p[n].y = A[i].y; p[n].z = A[i].z; p[n].c = color; }

	SET_NODE(0, 1, SG_COLOR_BLACK);
	SET_NODE(1, 2, SG_COLOR_BLACK);
	SET_NODE(2, 3, SG_COLOR_BLACK);
	Draw_Triangle(p, true);

	SET_NODE(0, 1, SG_COLOR_WHITE);
	SET_NODE(1, 2, SG_COLOR_WHITE);
	SET_NODE(2, 4, SG_COLOR_WHITE);
	Draw_Triangle(p, true);

	m_pDrape = pDrape;
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define LABEL_SCALE 50.

//---------------------------------------------------------
void CSG_3DView_Canvas::_Draw_Labels(int Front)
{
	if( m_Labels == 2 ) // none
	{
		return;
	}

	TSG_Point_3D Box[8]; _Draw_Get_Box(Box, false);

	switch( m_Labels )
	{
	default:
		switch( Front )
		{
		default:
			_Draw_Labels(0, Box[0], Box[1],   0,   0,   0, LABEL_ALIGN_LEFT , m_Label_Res, m_Label_Scale);
			_Draw_Labels(1, Box[3], Box[0], 270,   0,   0, LABEL_ALIGN_RIGHT, m_Label_Res, m_Label_Scale);
			_Draw_Labels(2, Box[3], Box[7], 180,  90, 270, LABEL_ALIGN_RIGHT, m_Label_Res, m_Label_Scale);
			_Draw_Labels(2, Box[1], Box[5], 180, 270, 180, LABEL_ALIGN_LEFT , m_Label_Res, m_Label_Scale);
			break;

		case  1:
			_Draw_Labels(0, Box[0], Box[1],   0,   0,   0, LABEL_ALIGN_RIGHT, m_Label_Res, m_Label_Scale);
			_Draw_Labels(1, Box[1], Box[2],  90,   0,   0, LABEL_ALIGN_LEFT , m_Label_Res, m_Label_Scale);
			_Draw_Labels(2, Box[0], Box[4], 180,  90, 180, LABEL_ALIGN_RIGHT, m_Label_Res, m_Label_Scale);
			_Draw_Labels(2, Box[2], Box[6], 180, 270, 270, LABEL_ALIGN_LEFT , m_Label_Res, m_Label_Scale);
			break;

		case  2:
			_Draw_Labels(0, Box[2], Box[3], 180,   0,   0, LABEL_ALIGN_LEFT , m_Label_Res, m_Label_Scale);
			_Draw_Labels(1, Box[1], Box[2],  90,   0,   0, LABEL_ALIGN_RIGHT, m_Label_Res, m_Label_Scale);
			_Draw_Labels(2, Box[1], Box[5], 180,  90,  90, LABEL_ALIGN_RIGHT, m_Label_Res, m_Label_Scale);
			_Draw_Labels(2, Box[3], Box[7], 180, 270,   0, LABEL_ALIGN_LEFT , m_Label_Res, m_Label_Scale);
			break;

		case  3:
			_Draw_Labels(0, Box[2], Box[3], 180,   0,   0, LABEL_ALIGN_RIGHT, m_Label_Res, m_Label_Scale);
			_Draw_Labels(1, Box[3], Box[0], 270,   0,   0, LABEL_ALIGN_LEFT , m_Label_Res, m_Label_Scale);
			_Draw_Labels(2, Box[2], Box[6], 180,  90,   0, LABEL_ALIGN_RIGHT, m_Label_Res, m_Label_Scale);
			_Draw_Labels(2, Box[0], Box[4], 180, 270,  90, LABEL_ALIGN_LEFT , m_Label_Res, m_Label_Scale);
			break;
		}
		break;

	case  1:
		_Draw_Labels(Box[0].x, Box[1].x, Box[0],   0, 0, 0, m_Label_Res, m_Label_Scale);
		_Draw_Labels(Box[1].y, Box[2].y, Box[1],  90, 0, 0, m_Label_Res, m_Label_Scale);
		_Draw_Labels(Box[2].x, Box[3].x, Box[2], 180, 0, 0, m_Label_Res, m_Label_Scale);
		_Draw_Labels(Box[3].y, Box[0].y, Box[3], 270, 0, 0, m_Label_Res, m_Label_Scale);
		break;
	}
}

//---------------------------------------------------------
void CSG_3DView_Canvas::_Draw_Labels(double Min, double Max, const TSG_Point_3D &Point, double Rx, double Ry, double Rz, int Resolution, double Scale)
{
	bool bAscending = Max > Min; if( !bAscending ) { double val = Min; Min = Max; Max = val; }

	Scale *= m_Projector.Get_Scale() / 1000.; Resolution /= 2;

	wxSize Size((int)((Max - Min) / Scale), Resolution);

	if( Size.GetWidth() < 1 || Size.GetHeight() < 1 )
	{
		return;
	}

	wxBitmap Bitmap(Size, 32); wxMemoryDC dc(Bitmap);

	wxColour FGColor(SG_GET_R(m_bgColor) + 128, SG_GET_G(m_bgColor) + 128, SG_GET_B(m_bgColor) + 128);
	wxColour BGColor(SG_GET_R(m_bgColor)      , SG_GET_G(m_bgColor)      , SG_GET_B(m_bgColor)      );

	dc.SetBackground(BGColor); dc.Clear(); dc.SetPen(FGColor); dc.SetTextForeground(FGColor);
	if( !m_bBox ) { dc.DrawLine(0, 0, Size.GetWidth(), 0); }

	Draw_Scale(dc, Size, Min, Max, true, bAscending, true);

	CSG_Vector P(3); P[0] = Point.x; P[1] = Point.y; P[2] = Point.z;
	CSG_Matrix R = SG_Matrix_Get_Rotation(Rx, Ry, Rz, true); R *= Scale;

	wxImage Image(Bitmap.ConvertToImage()); _Draw_Image(Image, P, R, m_bgColor);
}

//---------------------------------------------------------
void CSG_3DView_Canvas::_Draw_Labels(int Axis, const TSG_Point_3D &A, const TSG_Point_3D &B, double Rx, double Ry, double Rz, int Align, int Resolution, double Scale)
{
	double Min = Axis == 0 ? A.x : Axis == 1 ? A.y : A.z;
	double Max = Axis == 0 ? B.x : Axis == 1 ? B.y : B.z;

	if( Max <= Min ) { if( Max < Min ) { _Draw_Labels(Axis, B, A, Rx, Ry, Rz, Align, Resolution, Scale); } return; }

	//-----------------------------------------------------
	if( !m_bBox )
	{
		Draw_Line(m_Projector.Get_Projection(A), m_Projector.Get_Projection(B),
			SG_GET_RGB(SG_GET_R(m_bgColor) + 128, SG_GET_G(m_bgColor) + 128, SG_GET_B(m_bgColor) + 128)
		);
	}

	//-----------------------------------------------------
	double Step = pow(10., floor(log10(Max - Min)) - 1.); int Decimals = Step >= 1. ? 0 : (int)floor(-log10(Step));
	{
		double Width = Scale * m_Projector.Get_Scale() / LABEL_SCALE;
		
		if( Align == LABEL_ALIGN_TOP || Align == LABEL_ALIGN_BOTTOM )
		{
			wxString Text(wxString::Format("%.*f", Decimals, fabs(Min) > fabs(Max) ? Min : Max));

			Width *= 0.5 * Text.Length();
		}

		if( Axis != 2 && (Align == LABEL_ALIGN_LEFT || Align == LABEL_ALIGN_RIGHT) )
		{
			Width *= 2.;
		}

		if( Axis == 2 && m_Projector.Get_zScaling() )
		{
			Width /= fabs(m_Projector.Get_zScaling());
		}

		while( Step <= Width ) { Step *= 2.; }
	}

	//-----------------------------------------------------
	CSG_Point_3D D(B.x - A.x, B.y - A.y, B.z - A.z);

	for(double Value=Step*floor(Min/Step); !std::isinf(Value) && Value<=Max; Value+=Step)
	{
		if( Value < Min )
		{
			continue;
		}

		double i = (Value - Min) / (Max - Min); CSG_Point_3D p(A.x + i * D.x, A.y + i * D.y, A.z + i * D.z); CSG_String s;

		if( Axis == 2 && m_Label_zType == SG_DATATYPE_Date )
		{
			CSG_DateTime d(Value); s = d.Format_ISODate();
		}
		else
		{
			s = SG_Get_String(Value, -Decimals);
		}

		_Draw_Label(s, p, Rx, Ry, Rz, Align, Resolution, Scale);
	}
}

//---------------------------------------------------------
void CSG_3DView_Canvas::_Draw_Label(const CSG_String &Text, const TSG_Point_3D &Point, double Rx, double Ry, double Rz, int Align, int Resolution, double Scale)
{
	if( Text.is_Empty() || Resolution < 20 )
	{
		return;
	}

	Scale *= m_Projector.Get_Scale() / (Resolution * LABEL_SCALE); // normalizing the targeted label size

	wxSize Size; switch( Align )
	{
	case LABEL_ALIGN_TOP : case LABEL_ALIGN_BOTTOM: Size.y = Resolution; Size.x = (int)(Text.Length() * 0.5 * Resolution); break;
	case LABEL_ALIGN_LEFT: case LABEL_ALIGN_RIGHT : Size.x = Resolution; Size.y = (int)(Text.Length() * 0.5 * Resolution); break;
	}

	wxBitmap Bitmap(Size, 32); wxMemoryDC dc(Bitmap);
	wxColour FGColor(SG_GET_R(m_bgColor) + 128, SG_GET_G(m_bgColor) + 128, SG_GET_B(m_bgColor) + 128);
	wxColour BGColor(SG_GET_R(m_bgColor)      , SG_GET_G(m_bgColor)      , SG_GET_B(m_bgColor)      );
	dc.SetBackground(BGColor); dc.Clear(); dc.SetPen(FGColor); dc.SetTextForeground(FGColor);
	wxFont Font(dc.GetFont()); Font.SetPixelSize(wxSize(0, (int)(0.7 * Resolution))); dc.SetFont(Font);
//	dc.SetBrush(*wxTRANSPARENT_BRUSH); dc.DrawRectangle(0, 0, Size.x, Size.y);

	int Tick = (int)(0.2 * Resolution);
	dc.DrawLine(Size.x/2, 0, Size.x/2, Tick); // draw tick!

	switch( Align )
	{
	case LABEL_ALIGN_TOP   : Draw_Text(dc, TEXTALIGN_TOPCENTER   , Size.x/2, Tick      , Text.c_str()); break;
	case LABEL_ALIGN_BOTTOM: Draw_Text(dc, TEXTALIGN_BOTTOMCENTER, Size.x/2, Tick, 180., Text.c_str()); break;
	case LABEL_ALIGN_LEFT  : Draw_Text(dc, TEXTALIGN_CENTERLEFT  , Size.x/2, Tick, 270., Text.c_str()); break;
	case LABEL_ALIGN_RIGHT : Draw_Text(dc, TEXTALIGN_CENTERRIGHT , Size.x/2, Tick,  90., Text.c_str()); break;
	}

	double zScaling = m_Projector.Get_zScaling(); m_Projector.Set_zScaling(1.);
	CSG_Vector P(3); P[0] = Point.x; P[1] = Point.y; P[2] = m_Projector.Get_zCenter() + (Point.z - m_Projector.Get_zCenter()) * zScaling;
	CSG_Matrix R = SG_Matrix_Get_Rotation(Rx, Ry, Rz, true); R *= Scale;

	wxImage Image(Bitmap.ConvertToImage()); _Draw_Image(Image, P, R, m_bgColor, Size.x/2, 0);

	m_Projector.Set_zScaling(zScaling);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_3DView_Canvas::_Draw_Image(wxImage &Image, const CSG_Vector &Move, const CSG_Matrix &Rotate, int BGColor, int xOffset, int yOffset)
{
	#define GET_NODE(xImage, yImage, i) {\
		CSG_Vector v(3); v[0] = xImage - xOffset; v[1] = -(yImage - yOffset); v[2] = 0; v = Move + Rotate * v;\
		m_Projector.Get_Projection(v[0], v[1], v[2]);\
		p[i].x = v[0]; p[i].y = v[1]; p[i].z = v[2]; p[i].c = c[i];\
	}

	//-----------------------------------------------------
	CSG_Grid *pDrape = m_pDrape; m_pDrape = NULL;

	//-----------------------------------------------------
	#pragma omp parallel for
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

				GET_NODE(x0, y0, 0);
				GET_NODE(x1, y0, 1);
				GET_NODE(x1, y1, 2);
				GET_NODE(x0, y1, 3);

				int r = (SG_GET_R(c[0]) + SG_GET_R(c[1]) + SG_GET_R(c[2]) + SG_GET_R(c[3])) / 4;
				int g = (SG_GET_G(c[0]) + SG_GET_G(c[1]) + SG_GET_G(c[2]) + SG_GET_G(c[3])) / 4;
				int b = (SG_GET_B(c[0]) + SG_GET_B(c[1]) + SG_GET_B(c[2]) + SG_GET_B(c[3])) / 4;

				Triangle[2].x = (p[0].x + p[1].x + p[2].x + p[3].x) / 4.;
				Triangle[2].y = (p[0].y + p[1].y + p[2].y + p[3].y) / 4.;
				Triangle[2].z = (p[0].z + p[1].z + p[2].z + p[3].z) / 4.;
				Triangle[2].c = SG_GET_RGB(r, g, b);

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
//                                                       //
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
double CSG_3DView_Canvas::Get_Dim(double x0, double y0, double z0, double x1, double y1, double z1, double x2, double y2, double z2, const CSG_Vector &LightSource, int Shading, double zScale)
{
	CSG_Vector n(3), v(3);

	n[0] = x2 - x0; n[1] = y2 - y0; n[2] = (z2 - z0) * zScale;
	v[0] = x1 - x0; v[1] = y1 - y0; v[2] = (z1 - z0) * zScale;

	double dim = n.Get_Cross_Product(v).Get_Angle(LightSource) / M_PI_090;

	switch( Shading )
	{
	case  1: if( dim > 1. ) { dim = 2. - dim; }
		dim = 0.5 + 0.5 * dim; break;

	case  2:
		dim = 1.  - 0.8 * dim; break;
	}

	return( dim );
}

double CSG_3DView_Canvas::Get_Dim(const CSG_Point_3D &p0, const CSG_Point_3D &p1, const CSG_Point_3D &p2, const CSG_Vector &LightSource, int Shading, double zScale)
{
	return( Get_Dim(p0.x, p0.y, p0.z, p1.x, p1.y, p1.z, p2.x, p2.y, p2.z, LightSource, Shading, zScale) );
}

//---------------------------------------------------------
inline void CSG_3DView_Canvas::_Draw_Pixel(int x, int y, double z, int color)
{
	if( x >= 0 && x < m_Image_NX && y >= 0 && y < m_Image_NY && z >= 0. && z < m_Image_zMax[y][x] )
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
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_3DView_Canvas::Draw_Point(int x, int y, double z, int Color, int Size)
{
	if( z > 0. )
	{
		_Draw_Pixel(x, y, z, Color);

		if( Size > 0 )
		{
			if( Size > 50 ) Size = 50;

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
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_3DView_Canvas::Draw_Line(double ax, double ay, double az, double bx, double by, double bz, int aColor, int bColor)
{
	if(	(ax < 0. && bx < 0.) || ((int)ax >= m_Image_NX && (int)bx >= m_Image_NX)
	||	(ay < 0. && by < 0.) || ((int)ay >= m_Image_NY && (int)by >= m_Image_NY) )
	{
		return; // completely out of area
	}

	if( bz < 0. && az < 0. )
	{
		return; // completely in front of screen
	}

	double dx = bx - ax, dy = by - ay;

	if( fabs(dx) < 1. && fabs(dy) < 1. ) // both end points fall into the same pixel
	{
		_Draw_Pixel((int)ax, (int)ay, az < bz ? az : bz, az < bz ? aColor : bColor);
	}
	else
	{
		double dz = bz - az, ac = 0., dc = 1.; CSG_Colors Colors(2); Colors[0] = aColor; Colors[1] = bColor;

		if( fabs(dx) > fabs(dy) )
		{
			if( dx < 0. ) // ax < bx
			{
				double d = ax;
				ax = bx; dx = -dx; bx = d;
				ay = by; dy = -dy;
				az = bz; dz = -dz;
				ac = 1.; dc = -1.;
			}

			dy /= fabs(dx); dz /= fabs(dx); dc /= fabs(dx);

			if( ax < 0. ) { ay -= dy * ax; az -= dz * ax; ac -= dc * ax; ax = 0.; }
			if( bx >= m_Image_NX ) { bx = m_Image_NX - 1; }

			for(int ix=(int)ax; ix<=(int)bx; ix++, ay+=dy, az+=dz, ac+=dc)
			{
				_Draw_Pixel(ix, (int)ay, az, Colors.Get_Interpolated(ac));
			}
		}
		else
		{
			if( dy < 0. ) // ax < bx
			{
				double d = ay;
				ay = by; dy = -dy; by = d;
				ax = bx; dx = -dx;
				az = bz; dz = -dz;
				ac = 1.; dc = -1.;
			}

			dx /= fabs(dy); dz /= fabs(dy); dc /= fabs(dy);

			if( ay < 0. ) { ax -= dx * ay; az -= dz * ay; ac -= dc * ay; ay = 0.; }
			if( by >= m_Image_NY ) { by = m_Image_NY - 1; }

			for(int iy=(int)ay; iy<=(int)by; iy++, ax+=dx, az+=dz, ac+=dc)
			{
				_Draw_Pixel((int)ax, iy, az, Colors.Get_Interpolated(ac));
			}
		}
	}
}

void CSG_3DView_Canvas::Draw_Line(const TSG_Point_3D &a, const TSG_Point_3D &b, int aColor, int bColor)
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

void CSG_3DView_Canvas::Draw_Line(const TSG_Point_3D &a, const TSG_Point_3D &b, int Color)
{
	Draw_Line(a.x, a.y, a.z, b.x, b.y, b.z, Color);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_3DView_Canvas::Draw_Triangle(TSG_Triangle_Node p[3], bool bValueAsColor, const CSG_Vector &LightSource, int Shading, double zScale)
{
	Draw_Triangle(p, bValueAsColor, Get_Dim(p[0].x, p[0].y, p[0].z, p[1].x, p[1].y, p[1].z, p[2].x, p[2].y, p[2].z, LightSource, Shading, zScale));
}

//---------------------------------------------------------
#define TRIANGLE_SET_POINT(P, N) {\
	P[0] = N.x;\
	P[1] = N.y;\
	P[2] = N.z;\
	switch( Mode )\
	{\
	case 0:\
		P[3] = N.c;\
		break;\
	case 1:\
		P[3] = N.c;\
		P[4] = N.d;\
		break;\
	case 2:\
		P[3] = SG_GET_R((int)N.c);\
		P[4] = SG_GET_G((int)N.c);\
		P[5] = SG_GET_B((int)N.c);\
		break;\
	}\
}

//---------------------------------------------------------
#define TRIANGLE_GET_GRADIENT(A, B, D) if( (D[1] = B[1] - A[1]) <= 0. ) { D[0] = D[1] = D[2] = D[3] = D[4] = D[5] = 0.; } else {\
	D[0] = (B[0] - A[0]) / D[1];\
	D[2] = (B[2] - A[2]) / D[1];\
	switch( Mode )\
	{\
	case 0:\
		D[3] = (B[3] - A[3]) / D[1];\
		break;\
	case 1:\
		D[3] = (B[3] - A[3]) / D[1];\
		D[4] = (B[4] - A[4]) / D[1];\
		break;\
	case 2:\
		D[3] = (B[3] - A[3]) / D[1];\
		D[4] = (B[4] - A[4]) / D[1];\
		D[5] = (B[5] - A[5]) / D[1];\
		break;\
	}\
}

//---------------------------------------------------------
#define TRIANGLE_GET_POINT(A, D, P) { double dy = y - A[1]; P[1] = y;\
	P[0] = A[0] + D[0] * dy;\
	P[2] = A[2] + D[2] * dy;\
	switch( Mode )\
	{\
	case 0:\
		P[3] = A[3] + D[3] * dy;\
		break;\
	case 1:\
		P[3] = A[3] + D[3] * dy;\
		P[4] = A[4] + D[4] * dy;\
		break;\
	case 2:\
		P[3] = A[3] + D[3] * dy;\
		P[4] = A[4] + D[4] * dy;\
		P[5] = A[5] + D[5] * dy;\
		break;\
	}\
}

//---------------------------------------------------------
void CSG_3DView_Canvas::Draw_Triangle(TSG_Triangle_Node Point[3], bool bValueAsColor, double Dim)
{
	if( Point[0].z < 0. && Point[1].z < 0. && Point[2].z < 0. )
	{
		return;	// completely in front of projection plane
	}

	CSG_Rect r(Point[0].x, Point[0].y, Point[1].x, Point[1].y); r.Union(Point[2].x, Point[2].y);

	if( r.Get_XMax() < 0. || r.Get_XMin() >= m_Image_NX
	||  r.Get_YMax() < 0. || r.Get_YMin() >= m_Image_NY )
	{
		return;	// completely off screen
	}

	if( (r.Get_XMin() < 0. && r.Get_XMax() >= m_Image_NX)
	||  (r.Get_YMin() < 0. && r.Get_YMax() >= m_Image_NY) )
	{
		return;	// completely filling the screen
	}

	if( r.Get_XRange() <= 0. || r.Get_YRange() <= 0. )
	{
		return;	// has no area (... should draw a point ?!)
	}

	//-----------------------------------------------------
	int i[3], Mode = m_pDrape ? 1 : bValueAsColor ? 2 : 0;

	if     ( Point[0].y < Point[1].y && Point[0].y < Point[2].y )
	{
		i[0] = 0; if( Point[1].y < Point[2].y ) { i[1] = 1; i[2] = 2; } else { i[1] = 2; i[2] = 1; }
	}
	else if( Point[1].y < Point[0].y && Point[1].y < Point[2].y )
	{
		i[0] = 1; if( Point[0].y < Point[2].y ) { i[1] = 0; i[2] = 2; } else { i[1] = 2; i[2] = 0; }
	}
	else // if( Point[2].y < Point[0].y && Point[2].y < Point[1].y )
	{
		i[0] = 2; if( Point[0].y < Point[1].y ) { i[1] = 0; i[2] = 1; } else { i[1] = 1; i[2] = 0; }
	}

	double p[3][6], tm[6], tb[6], mb[6];

	TRIANGLE_SET_POINT(p[0], Point[i[0]]); // top
	TRIANGLE_SET_POINT(p[1], Point[i[1]]); // middle
	TRIANGLE_SET_POINT(p[2], Point[i[2]]); // bottom

	TRIANGLE_GET_GRADIENT(p[0], p[1], tm); // top to middle
	TRIANGLE_GET_GRADIENT(p[0], p[2], tb); // top to bottom
	TRIANGLE_GET_GRADIENT(p[1], p[2], mb); // middle to bottom

	int ay = (int)(r.Get_YMin()); if( ay < 0           ) ay = 0; if( ay < r.Get_YMin() ) ay++;
	int by = (int)(r.Get_YMax()); if( by >= m_Image_NY ) by = m_Image_NY - 1;

	//-----------------------------------------------------
	for(int y=ay; y<=by; y++)
	{
		if( y <= p[1][1] )
		{
			if( tm[1] > 0. )
			{
				double a[6]; TRIANGLE_GET_POINT(p[0], tb, a); // a = p[0] + tb * (y - p[0][1]); // using CSG_Vector is, unluckily, significantly slower!!
				double b[6]; TRIANGLE_GET_POINT(p[0], tm, b);

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
		else
		{
			if( mb[1] > 0. )
			{
				double a[6]; TRIANGLE_GET_POINT(p[0], tb, a);
				double b[6]; TRIANGLE_GET_POINT(p[1], mb, b);

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
}

//---------------------------------------------------------
inline void CSG_3DView_Canvas::_Draw_Triangle_Line(int y, double a[], double b[], double Dim, int Mode)
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
	double d[6]; d[0] = b[0] - a[0];

	switch( Mode )
	{
	case 2:
		d[5] = (b[5] - a[5]) / d[0];
	case 1:
		d[4] = (b[4] - a[4]) / d[0];
	default:
		d[3] = (b[3] - a[3]) / d[0];
		d[2] = (b[2] - a[2]) / d[0];
	}

	//-----------------------------------------------------
	int ax = (int)(a[0]); if( ax <  0          ) ax = 0; double dx = ax - a[0];
	int bx = (int)(b[0]); if( bx >= m_Image_NX ) bx = m_Image_NX - 1;

	for(int x=ax; x<=bx; x++, dx++)
	{
		double z = a[2] + dx * d[2];

		switch( Mode )
		{
		default: {
			_Draw_Pixel(x, y, z, _Dim_Color(Get_Color(a[3] + dx * d[3]), Dim));
			break; }

		case 1: {
			double Value;

			if( m_pDrape->Get_Value(a[3] + dx * d[3], a[4] + dx * d[4], Value, m_Drape_Mode, false, true) )
			{
				_Draw_Pixel(x, y, z, _Dim_Color((int)Value, Dim));
			}
			break; }

		case 2: {
			int r = (int)(a[3] + dx * d[3]); if( r < 0 ) { r = 0; } else if( r > 255 ) { r = 255; }
			int g = (int)(a[4] + dx * d[4]); if( g < 0 ) { g = 0; } else if( g > 255 ) { g = 255; }
			int b = (int)(a[5] + dx * d[5]); if( b < 0 ) { b = 0; } else if( b > 255 ) { b = 255; }

			_Draw_Pixel(x, y, z, _Dim_Color(SG_GET_RGB(r, g, b), Dim));
			break; }
		}
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool Get_Polygon_Plane(const CSG_Shape_Polygon &Polygon, int iPart, CSG_Vector &Normal, double zScale)
{
	if( Polygon.is_Valid() )
	{
		CSG_Vector v[2]; int n = 0; CSG_Point_3D a = Polygon.Get_Point_Z(0, iPart, false);

		for(int i=0; n<2 && i<Polygon.Get_Point_Count(iPart); i++)
		{
			CSG_Point_3D b = a; a = Polygon.Get_Point_Z(i, iPart);

			v[n].Create(3); v[n][0] = a.x - b.x; v[n][1] = a.y - b.y; v[n][2] = (a.z - b.z);// * zScale;

			if( !v[n].is_Null() && (n == 0 || !v[0].is_Collinear(v[1])) )
			{
				n++;
			}
		}

		if( n == 2 )
		{
			Normal = v[0].Get_Cross_Product(v[1]); // cross product v0 x v1 => normal vector

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
void CSG_3DView_Canvas::Draw_Polygon(CSG_Shape_Polygon &Polygon, int Color, const CSG_Vector &LightSource, int Shading, double zScale)
{
	CSG_Vector Normal; if( !Get_Polygon_Plane(Polygon, 0, Normal, zScale) ) { return; }

	double dim = Normal.Get_Angle(LightSource) / M_PI_090;

	switch( Shading )
	{
	case  1: if( dim > 1. ) { dim = 2. - dim; }
		dim = 0.5 + 0.5 * dim; break;

	case  2:
		dim = 1.  - 0.8 * dim; break;
	}

	Draw_Polygon(Polygon, _Dim_Color(Color, dim));
}

//---------------------------------------------------------
void CSG_3DView_Canvas::Draw_Polygon(CSG_Shape_Polygon &Polygon, int Color)
{
	for(int iPart=0; iPart<Polygon.Get_Part_Count(); iPart++)
	{
		CSG_Vector Plane;

		if( Get_Polygon_Plane(Polygon, iPart, Plane, 1) )
		{
			CSG_Point_3D p = Polygon.Get_Point_Z(0, iPart), n(Plane[0], Plane[1], Plane[2]);

			CSG_Rect r(0, 0, m_Image_NX, m_Image_NY); r.Intersect(Polygon.Get_Part(iPart)->Get_Extent());

			#pragma omp parallel for
			for(int y=(int)r.Get_YMin(); y<=(int)r.Get_YMax(); y++)
			{
				for(int x=(int)r.Get_XMin(); x<=(int)r.Get_XMax(); x++)
				{
					if( Polygon.Contains(x, y, iPart) )
					{
						double z = p.z - ((x - p.x) * n.x + (y - p.y) * n.y) / n.z;

						_Draw_Pixel(x, y, z, Color);
					}
				}
			}
		}
	}
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
