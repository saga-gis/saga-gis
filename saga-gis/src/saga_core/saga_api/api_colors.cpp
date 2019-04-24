/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_API                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   api_colors.cpp                      //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'.                              //
//                                                       //
// This library is free software; you can redistribute   //
// it and/or modify it under the terms of the GNU Lesser //
// General Public License as published by the Free       //
// Software Foundation, either version 2.1 of the        //
// License, or (at your option) any later version.       //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, see <http://www.gnu.org/licenses/>.              //
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
//					class CSG_Colors					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <stdlib.h>
#include <string.h>

#include "api_core.h"
#include "mat_tools.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
long	SG_Color_Get_Random(void)
{
	return(	SG_GET_RGB(CSG_Random::Get_Uniform(0, 255), CSG_Random::Get_Uniform(0, 255), CSG_Random::Get_Uniform(0, 255)) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define COLORS_SERIAL_VERSION_BINARY	"SAGA_COLORPALETTE_VERSION_0.100_BINARY"
#define COLORS_SERIAL_VERSION__ASCII	"SAGA_COLORPALETTE_VERSION_0.100__ASCII"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String		SG_Colors_Get_Name	(int Identifier)
{
	switch( Identifier )
	{
	case SG_COLORS_DEFAULT        :	return( _TL("default") );
	case SG_COLORS_DEFAULT_BRIGHT :	return( _TL("default (same brightness)") );
	case SG_COLORS_BLACK_WHITE    :	return( _TL("greyscale") );
	case SG_COLORS_BLACK_RED      :	return( _TL("black > red") );
	case SG_COLORS_BLACK_GREEN    :	return( _TL("black > green") );
	case SG_COLORS_BLACK_BLUE     :	return( _TL("black > blue") );
	case SG_COLORS_WHITE_RED      :	return( _TL("white > red") );
	case SG_COLORS_WHITE_GREEN    :	return( _TL("white > green") );
	case SG_COLORS_WHITE_BLUE     :	return( _TL("white > blue") );
	case SG_COLORS_YELLOW_RED     :	return( _TL("yellow > red") );
	case SG_COLORS_YELLOW_GREEN   :	return( _TL("yellow > green") );
	case SG_COLORS_YELLOW_BLUE    :	return( _TL("yellow > blue") );
	case SG_COLORS_RED_GREEN      :	return( _TL("red > green") );
	case SG_COLORS_RED_BLUE       :	return( _TL("red > blue") );
	case SG_COLORS_GREEN_BLUE     :	return( _TL("green > blue") );
	case SG_COLORS_RED_GREY_BLUE  :	return( _TL("red > grey > blue") );
	case SG_COLORS_RED_GREY_GREEN :	return( _TL("red > grey > green") );
	case SG_COLORS_GREEN_GREY_BLUE:	return( _TL("green > grey > blue") );
	case SG_COLORS_RED_GREEN_BLUE :	return( _TL("red > green > blue") );
	case SG_COLORS_RED_BLUE_GREEN :	return( _TL("red > blue > green") );
	case SG_COLORS_GREEN_RED_BLUE :	return( _TL("green > red > blue") );
	case SG_COLORS_RAINBOW        :	return( _TL("rainbow") );
	case SG_COLORS_NEON           :	return( _TL("neon") );
	case SG_COLORS_TOPOGRAPHY     :	return( _TL("topography 1") );
	case SG_COLORS_TOPOGRAPHY_2   :	return( _TL("topography 2") );
	case SG_COLORS_TOPOGRAPHY_3   :	return( _TL("topography 3") );
	case SG_COLORS_PRECIPITATION  :	return( _TL("precipitation") );
	case SG_COLORS_ASPECT_1       :	return( _TL("aspect 1") );
	case SG_COLORS_ASPECT_2       :	return( _TL("aspect 2") );
	case SG_COLORS_ASPECT_3       :	return( _TL("aspect 3") );
	}

	return( "" );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Colors::CSG_Colors(void)
{
	m_Colors	= NULL;
	m_nColors	= 0;

	Create();
}

//---------------------------------------------------------
CSG_Colors::CSG_Colors(const CSG_Colors &Colors)
{
	m_Colors	= NULL;
	m_nColors	= 0;

	Create(Colors);
}

//---------------------------------------------------------
CSG_Colors::CSG_Colors(int nColors, int Palette, bool bRevert)
{
	m_Colors	= NULL;
	m_nColors	= 0;

	Create(nColors, Palette, bRevert);
}

//---------------------------------------------------------
CSG_Colors::~CSG_Colors(void)
{
	Destroy();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Colors::Create(void)
{
	return( Create(11) );
}

//---------------------------------------------------------
bool CSG_Colors::Create(const CSG_Colors &Colors)
{
	if( Colors.m_nColors > 0 )
	{
		m_nColors	= Colors.m_nColors;
		m_Colors	= (long *)SG_Realloc(m_Colors, m_nColors * sizeof(long));

		memcpy(m_Colors, Colors.m_Colors, m_nColors * sizeof(long));

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Colors::Create(int nColors, int Palette, bool bRevert)
{
	if( nColors <= 1 )
	{
		nColors	= 11;
	}

	Set_Count(nColors);

	Set_Palette(Palette, bRevert, nColors);

	return( true );
}

//---------------------------------------------------------
void CSG_Colors::Destroy(void)
{
	if( m_nColors > 0 )
	{
		SG_Free(m_Colors);

		m_Colors	= NULL;
		m_nColors	= 0;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Colors::Set_Count(int nColors)
{
	if( nColors == m_nColors )
	{
		return( true );
	}

	if( nColors < 1 )
	{
		return( false );
	}

	if( m_nColors == 0 )
	{
		return( Set_Default(nColors) );
	}

	//-----------------------------------------------------
	CSG_Colors	Colors(*this);

	m_nColors	= nColors;
	m_Colors	= (long *)SG_Realloc(m_Colors, m_nColors * sizeof(long));

	double	dStep	= Get_Count() > 1 ? (Colors.Get_Count() - 1.0) / (Get_Count() - 1.0) : 0.0;

	for(int i=0; i<Get_Count(); i++)
	{
		if( Get_Count() < Colors.Get_Count() )
		{
			m_Colors[i]	= Colors[(int)(i * dStep)];
		}
		else // if( Get_Count() > Colors.Get_Count() )
		{
			m_Colors[i]	= Colors.Get_Interpolated(i * dStep);
		}
	}

	//---------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Colors::Set_Color(int Index, long Color)
{
	if( Index >= 0 && Index < m_nColors )
	{
		m_Colors[Index]	= Color;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Colors::Set_Color(int Index, int Red, int Green, int Blue)
{
	return( Set_Color(Index, SG_GET_RGB(Red, Green, Blue)) );
}

//---------------------------------------------------------
bool CSG_Colors::Set_Red(int Index, int Value)
{
	return( Set_Color(Index, Value			, Get_Green(Index)	, Get_Blue(Index)) );
}

//---------------------------------------------------------
bool CSG_Colors::Set_Green(int Index, int Value)
{
	return( Set_Color(Index, Get_Red(Index)	, Value				, Get_Blue(Index)) );
}

//---------------------------------------------------------
bool CSG_Colors::Set_Blue(int Index, int Value)
{
	return( Set_Color(Index, Get_Red(Index)	, Get_Green(Index)	, Value) );
}

//---------------------------------------------------------
bool CSG_Colors::Set_Brightness(int Index, int Value)
{
	double	r, g, b, ds;

	//-----------------------------------------------------
	if( Value < 0 )
	{
		Value	= 0;
	}
	else if( Value > 255 )
	{
		Value	= 255;
	}

	//-----------------------------------------------------
	r	= Get_Red  (Index);
	g	= Get_Green(Index);
	b	= Get_Blue (Index);
	ds	= (r + g + b) / 3.0;

	if( ds > 0.0 )
	{
		ds	= Value / ds;
		r	*= ds;
		g	*= ds;
		b	*= ds;

		_Set_Brightness(r, g, b);
	}
	else
	{
		r	= g	= b	= Value / 3.0;
	}

	return( Set_Color(Index, (int)r, (int)g, (int)b) );
}

//---------------------------------------------------------
void CSG_Colors::_Set_Brightness(double &a, double &b, double &c, int Pass)
{
	if( a > 255 )
	{
		int		addSum;

		addSum	= (int)((a - 255) / 2.0);
		a		= 255;

		b		+= addSum;
		c		+= addSum;

		if( b > 255 )
		{
			addSum	= (int)(b - 255);
			b		= 255;

			c		+= addSum;

			if( c > 255 )
			{
				c	= 255;
			}
		}
		else if( c > 255 )
		{
			addSum	= (int)(c - 255);
			c		= 255;

			b		+= addSum;

			if( b > 255 )
			{
				b	= 255;
			}
		}
	}
	else if( Pass < 2 )
	{
		_Set_Brightness(b, c, a, Pass + 1);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Colors::Set_Default(int nColors)
{
	int		i;
	double	d, dStep;

	if( nColors > 0 )
	{
		m_nColors	= nColors;
		m_Colors	= (long *)SG_Realloc(m_Colors, m_nColors * sizeof(long));

		dStep		= 2 * M_PI / (double)Get_Count();

		for(i=0, d=0; i<Get_Count(); i++, d+=dStep)
		{
			Set_Color(i,
				(int)(d < M_PI / 2 ? 0 : 128 - 127 * sin(M_PI - d)),
				(int)(128 - 127 * cos(d)),
				(int)(d > M_PI * 3 / 2 ? 0 : 128 + 127 * sin(d))
			);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Colors::Set_Palette(int Index, bool bRevert, int nColors)
{
	switch( Index )
	{
	default:
		return( false );

	case SG_COLORS_DEFAULT:
		Set_Default(nColors);
		break;

	case SG_COLORS_DEFAULT_BRIGHT:
		Set_Default(nColors);
		Set_Ramp_Brighness(127, 127);
		break;

	case SG_COLORS_BLACK_WHITE:
		Set_Ramp(SG_GET_RGB(  0,   0,   0), SG_GET_RGB(255, 255, 255));
		break;

	case SG_COLORS_BLACK_RED:
		Set_Ramp(SG_GET_RGB(  0,   0,   0), SG_GET_RGB(255,   0,   0));
		break;

	case SG_COLORS_BLACK_GREEN:
		Set_Ramp(SG_GET_RGB(  0,   0,   0), SG_GET_RGB(  0, 255,   0));
		break;

	case SG_COLORS_BLACK_BLUE:
		Set_Ramp(SG_GET_RGB(  0,   0,   0), SG_GET_RGB(  0,   0, 255));
		break;

	case SG_COLORS_WHITE_RED:
		Set_Count(3);
		Set_Color(0, SG_GET_RGB(255, 255, 255));
		Set_Color(1, SG_GET_RGB(255, 127,   0));
		Set_Color(2, SG_GET_RGB(159,   0,   0));
		break;

	case SG_COLORS_WHITE_GREEN:
		Set_Ramp(SG_GET_RGB(255, 255, 255), SG_GET_RGB(  0, 127,   0));
		break;

	case SG_COLORS_WHITE_BLUE:
		Set_Count(3);
		Set_Color(0, SG_GET_RGB(255, 255, 255));
		Set_Color(1, SG_GET_RGB(  0, 127, 255));
		Set_Color(2, SG_GET_RGB(  0,   0, 159));
		break;

	case SG_COLORS_YELLOW_RED:
		Set_Ramp(SG_GET_RGB(255, 255,   0), SG_GET_RGB(191,   0,   0));
		break;

	case SG_COLORS_YELLOW_GREEN:
		Set_Ramp(SG_GET_RGB(255, 255,   0), SG_GET_RGB(  0,  63,   0));
		break;

	case SG_COLORS_YELLOW_BLUE:
		Set_Count(3);
		Set_Color(2, SG_GET_RGB(  0,  64, 127));
		Set_Color(1, SG_GET_RGB(127, 192, 255));
		Set_Color(0, SG_GET_RGB(255, 255, 200));
		break;

	case SG_COLORS_RED_GREEN:
		Set_Count(5);
		Set_Color(0, SG_GET_RGB(159,   0,   0));
		Set_Color(1, SG_GET_RGB(255, 159,   0));
		Set_Color(2, SG_GET_RGB(255, 255,   0));
		Set_Color(3, SG_GET_RGB(159, 255,   0));
		Set_Color(4, SG_GET_RGB(  0, 159,   0));
		break;

	case SG_COLORS_RED_BLUE:
		Set_Ramp(SG_GET_RGB(255,   0,   0), SG_GET_RGB(  0,   0, 255));
		break;

	case SG_COLORS_GREEN_BLUE:
		Set_Ramp(SG_GET_RGB(  0, 255,   0), SG_GET_RGB(  0,   0, 255));
		break;

	case SG_COLORS_RED_GREY_BLUE:
		Set_Count(5);
		Set_Color(0, SG_GET_RGB(127,   0,   0));
		Set_Color(1, SG_GET_RGB(255, 127,   0));
		Set_Color(2, SG_GET_RGB(239, 239, 239));
		Set_Color(3, SG_GET_RGB(  0, 127, 255));
		Set_Color(4, SG_GET_RGB(  0,   0, 127));
		break;

	case SG_COLORS_RED_GREY_GREEN:
		Set_Count(5);
		Set_Color(0, SG_GET_RGB(127,   0,   0));
		Set_Color(1, SG_GET_RGB(255, 127,   0));
		Set_Color(2, SG_GET_RGB(239, 239, 239));
		Set_Color(3, SG_GET_RGB(  0, 255, 127));
		Set_Color(4, SG_GET_RGB(  0, 127,   0));
		break;

	case SG_COLORS_GREEN_GREY_BLUE:
		Set_Count(5);
		Set_Color(0, SG_GET_RGB(  0, 127,   0));
		Set_Color(1, SG_GET_RGB(127, 255,   0));
		Set_Color(2, SG_GET_RGB(239, 239, 239));
		Set_Color(3, SG_GET_RGB(  0, 127, 255));
		Set_Color(4, SG_GET_RGB(  0,   0, 127));
		break;

	case SG_COLORS_RED_GREEN_BLUE:
		Set_Count(5);
		Set_Color(0, SG_GET_RGB(127,   0, 127));
		Set_Color(1, SG_GET_RGB(255,   0,   0));
		Set_Color(2, SG_GET_RGB(  0, 255,   0));
		Set_Color(3, SG_GET_RGB(  0,   0, 255));
		Set_Color(4, SG_GET_RGB(127,   0, 127));
		break;

	case SG_COLORS_RED_BLUE_GREEN:
		Set_Count(5);
		Set_Color(0, SG_GET_RGB(127, 127,   0));
		Set_Color(1, SG_GET_RGB(255,   0,   0));
		Set_Color(2, SG_GET_RGB(  0,   0, 255));
		Set_Color(3, SG_GET_RGB(  0, 255,   0));
		Set_Color(4, SG_GET_RGB(127, 127,   0));
		break;

	case SG_COLORS_GREEN_RED_BLUE:
		Set_Count(5);
		Set_Color(0, SG_GET_RGB(  0, 127, 127));
		Set_Color(1, SG_GET_RGB(  0, 255,   0));
		Set_Color(2, SG_GET_RGB(255,   0,   0));
		Set_Color(3, SG_GET_RGB(  0,   0, 255));
		Set_Color(4, SG_GET_RGB(  0, 127, 127));
		break;

	case SG_COLORS_RAINBOW:
		Set_Count(8);
		Set_Color(0, SG_GET_RGB( 64,   0, 127));
		Set_Color(1, SG_GET_RGB(  0,   0, 255));
		Set_Color(2, SG_GET_RGB(  0, 255, 255));
		Set_Color(3, SG_GET_RGB(  0, 191,   0));
		Set_Color(4, SG_GET_RGB(255, 255,   0));
		Set_Color(5, SG_GET_RGB(255, 127,   0));
		Set_Color(6, SG_GET_RGB(255,   0,   0));
		Set_Color(7, SG_GET_RGB(127,   0,   0));
		break;

	case SG_COLORS_NEON:
		Set_Count(7);
		Set_Color(0, SG_GET_RGB(  0,   0,   0));
		Set_Color(1, SG_GET_RGB(255,   0,   0));
		Set_Color(2, SG_GET_RGB(  0,   0,   0));
		Set_Color(3, SG_GET_RGB(255, 255,   0));
		Set_Color(4, SG_GET_RGB(  0,   0,   0));
		Set_Color(5, SG_GET_RGB(  0, 255,   0));
		Set_Color(6, SG_GET_RGB(  0,   0,   0));
		break;

	case SG_COLORS_TOPOGRAPHY:
		Set_Count(5);
		Set_Color(0, SG_GET_RGB(  0,  63, 127));
		Set_Color(1, SG_GET_RGB(127, 255,   0));
		Set_Color(2, SG_GET_RGB(255, 255, 127));
		Set_Color(3, SG_GET_RGB(191, 127,   0));
		Set_Color(4, SG_GET_RGB(127,  63,   0));
		break;

	case SG_COLORS_TOPOGRAPHY_2:
		Set_Count(3);
		Set_Color(0, SG_GET_RGB(  0, 128,   0));
		Set_Color(1, SG_GET_RGB(255, 255, 127));
		Set_Color(2, SG_GET_RGB(127,  63,  63));
		break;

	case SG_COLORS_TOPOGRAPHY_3:
		Set_Count(9);
		Set_Color(0, SG_GET_RGB(177, 242, 212));
		Set_Color(1, SG_GET_RGB(248, 252, 179));
		Set_Color(2, SG_GET_RGB( 11, 128, 064));
		Set_Color(3, SG_GET_RGB(248, 202,  80));
		Set_Color(4, SG_GET_RGB(158,  30,   0));
		Set_Color(5, SG_GET_RGB(128, 064, 064));
		Set_Color(6, SG_GET_RGB(185, 121, 076));
		Set_Color(7, SG_GET_RGB(179, 179, 179));
		Set_Color(8, SG_GET_RGB(255, 255, 255));
		break;

	case SG_COLORS_PRECIPITATION:	// juergen's favorite precipition colour ramp
		Set_Count(22);
		Set_Color( 0, SG_GET_RGB(216, 204, 131));
		Set_Color( 1, SG_GET_RGB(196, 208, 111));
		Set_Color( 2, SG_GET_RGB(184, 210, 101));
		Set_Color( 3, SG_GET_RGB(172, 212,  91));
		Set_Color( 4, SG_GET_RGB(139, 212,  99));
		Set_Color( 5, SG_GET_RGB(107, 212, 107));
		Set_Color( 6, SG_GET_RGB( 75, 212, 119));
		Set_Color( 7, SG_GET_RGB( 42, 212, 131));
		Set_Color( 8, SG_GET_RGB( 26, 212, 151));
		Set_Color( 9, SG_GET_RGB( 10, 212, 172));
		Set_Color(10, SG_GET_RGB( 30, 192, 192));
		Set_Color(11, SG_GET_RGB( 50, 172, 212));
		Set_Color(12, SG_GET_RGB( 70, 151, 214));
		Set_Color(13, SG_GET_RGB( 91, 131, 216));
		Set_Color(14, SG_GET_RGB( 75, 115, 198));
		Set_Color(15, SG_GET_RGB( 58,  99, 180));
		Set_Color(16, SG_GET_RGB( 42,  83, 184));
		Set_Color(17, SG_GET_RGB( 26, 066, 188));
		Set_Color(18, SG_GET_RGB( 26, 046, 180));
		Set_Color(19, SG_GET_RGB( 26, 026, 166));
		Set_Color(20, SG_GET_RGB( 38,  18, 151));
		Set_Color(21, SG_GET_RGB( 50, 010, 131));
		break;

	case SG_COLORS_ASPECT_1:
		Set_Count(5);
		Set_Color(0, SG_GET_RGB(225, 225, 225));
		Set_Color(1, SG_GET_RGB(127, 127, 255));
		Set_Color(2, SG_GET_RGB( 20,  20,  20));
		Set_Color(3, SG_GET_RGB(127, 255, 127));
		Set_Color(4, SG_GET_RGB(225, 225, 225));
		break;

	case SG_COLORS_ASPECT_2:
		Set_Count(5);
		Set_Color(0, SG_GET_RGB(225, 225, 225));
		Set_Color(1, SG_GET_RGB(255, 127, 127));
		Set_Color(2, SG_GET_RGB( 20,  20,  20));
		Set_Color(3, SG_GET_RGB(127, 255, 127));
		Set_Color(4, SG_GET_RGB(225, 225, 225));
		break;

	case SG_COLORS_ASPECT_3:
		Set_Count(5);
		Set_Color(0, SG_GET_RGB(225, 225, 225));
		Set_Color(1, SG_GET_RGB(255, 127, 127));
		Set_Color(2, SG_GET_RGB( 20,  20,  20));
		Set_Color(3, SG_GET_RGB(127, 127, 255));
		Set_Color(4, SG_GET_RGB(225, 225, 225));
		break;
	}

	//-----------------------------------------------------
	Set_Count(nColors);

	if( bRevert )
	{
		Revert();
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Colors::Set_Ramp(long Color_A, long Color_B)
{
	return( Set_Ramp(Color_A, Color_B, 0, Get_Count() - 1) );
}

//---------------------------------------------------------
bool CSG_Colors::Set_Ramp(long Color_A, long Color_B, int iColor_A, int iColor_B)
{
	int		i, n, ar, ag, ab;
	double	dr, dg, db;

	//-----------------------------------------------------
	if( iColor_A > iColor_B )
	{
		i			= iColor_A;
		iColor_A	= iColor_B;
		iColor_B	= i;
	}

	if( iColor_A < 0 )
	{
		iColor_A	= 0;
	}

	if( iColor_B >= Get_Count() )
	{
		iColor_B	= Get_Count() - 1;
	}

	//-----------------------------------------------------
	if( (n = iColor_B - iColor_A) > 0 )
	{
		ar		= SG_GET_R(Color_A);
		dr		= (double)(SG_GET_R(Color_B) - ar) / (double)n;

		ag		= SG_GET_G(Color_A);
		dg		= (double)(SG_GET_G(Color_B) - ag) / (double)n;

		ab		= SG_GET_B(Color_A);
		db		= (double)(SG_GET_B(Color_B) - ab) / (double)n;

		for(i=0; i<=n; i++)
		{
			Set_Color(iColor_A + i,
				(int)(ar + i * dr),
				(int)(ag + i * dg),
				(int)(ab + i * db)
			);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Colors::Set_Ramp_Brighness(int Brightness_A, int Brightness_B)
{
	return( Set_Ramp_Brighness(Brightness_A, Brightness_B, 0, Get_Count() - 1) );
}

//---------------------------------------------------------
bool CSG_Colors::Set_Ramp_Brighness(int Brightness_A, int Brightness_B, int iColor_A, int iColor_B)
{
	int		i, n;
	double	dBrightness;

	//-----------------------------------------------------
	if( iColor_A > iColor_B )
	{
		i			= iColor_A;
		iColor_A	= iColor_B;
		iColor_B	= i;
	}

	if( iColor_A < 0 )
	{
		iColor_A	= 0;
	}

	if( iColor_B >= Get_Count() )
	{
		iColor_B	= Get_Count() - 1;
	}

	//-----------------------------------------------------
	if( (n = iColor_B - iColor_A) > 0 )
	{
		dBrightness	= (double)(Brightness_B - Brightness_A) / (double)n;

		for(i=0; i<=n; i++)
		{
			Set_Brightness(iColor_A + i, (int)(Brightness_A + i * dBrightness));
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Colors::Random(void)
{
	for(int i=0; i<Get_Count(); i++)
	{
		Set_Color(i,
			(int)(255.0 * (double)rand() / (double)RAND_MAX),
			(int)(255.0 * (double)rand() / (double)RAND_MAX),
			(int)(255.0 * (double)rand() / (double)RAND_MAX)
		);
	}

	return( Get_Count() > 0 );
}

//---------------------------------------------------------
bool CSG_Colors::Invert(void)
{
	for(int i=0; i<Get_Count(); i++)
	{
		Set_Color(i, 255 - Get_Red(i), 255 - Get_Green(i), 255 - Get_Blue(i));
	}

	return( Get_Count() > 0 );
}

//---------------------------------------------------------
bool CSG_Colors::Revert(void)
{
	for(int i=0, j=Get_Count()-1; i<j; i++, j--)
	{
		long    c  = Get_Color(j);
		Set_Color(j, Get_Color(i));
		Set_Color(i, c);
	}

	return( Get_Count() > 0 );
}

//---------------------------------------------------------
bool CSG_Colors::Greyscale(void)
{
	for(int i=0; i<Get_Count(); i++)
	{
		long	c	= Get_Brightness(i);

		Set_Color(i, c, c, c);
	}

	return( Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Colors & CSG_Colors::operator = (const CSG_Colors &Colors)
{
	Create(Colors);

	return( *this );
}

//---------------------------------------------------------
bool CSG_Colors::Assign(const CSG_Colors &Colors)
{
	return( Create(Colors) );
}

//---------------------------------------------------------
bool CSG_Colors::Assign(CSG_Colors *pColors)
{
	return( pColors ? Create(*pColors) : false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Colors::Load(const CSG_String &File_Name)
{
	CSG_File	Stream;

	if( !Stream.Open(File_Name, SG_FILE_R, true) )
	{
		return( false );
	}

	CSG_String	Version;

	Stream.Read(Version, sizeof(COLORS_SERIAL_VERSION__ASCII));

	//-----------------------------------------------------
	if( Version.Find(COLORS_SERIAL_VERSION__ASCII) == 0 )
	{
		return( Serialize(Stream, false, false) );
	}

	//-----------------------------------------------------
	Stream.Seek_Start();
	Stream.Read(Version, sizeof(COLORS_SERIAL_VERSION_BINARY));

	if( Version.Find(COLORS_SERIAL_VERSION_BINARY) == 0 )
	{
		int	nColors;

		Stream.Read(&nColors, sizeof(nColors));

		if( Set_Count(nColors) )	// different os, different sizeof(long) !!
		{
			size_t	ValueSize	= (size_t)((Stream.Length() - (sizeof(COLORS_SERIAL_VERSION_BINARY) + sizeof(int))) / nColors);

			if( ValueSize > 0 )
			{
				BYTE	*c	= (BYTE *)SG_Malloc(ValueSize);

				for(int i=0; i<nColors; i++)
				{
					Stream.Read(c, ValueSize);

					Set_Color(i, c[0], c[1], c[2]);
				}

				SG_Free(c);
			}

			return( true );
		}
	}

	//-----------------------------------------------------
	else // SAGA 1.x compatibility...
	{
		short	nColors;

		Stream.Seek_Start();
		Stream.Read(&nColors, sizeof(short));

		if( Stream.Length() == (int)(sizeof(short) + 3 * nColors) && Set_Count(nColors) )
		{
			BYTE *R = (BYTE *)SG_Malloc(nColors * sizeof(BYTE)); Stream.Read(R, nColors * sizeof(BYTE));
			BYTE *G = (BYTE *)SG_Malloc(nColors * sizeof(BYTE)); Stream.Read(G, nColors * sizeof(BYTE));
			BYTE *B = (BYTE *)SG_Malloc(nColors * sizeof(BYTE)); Stream.Read(B, nColors * sizeof(BYTE));

			for(int i=0; i<nColors; i++)
			{
				Set_Color(i, R[i], G[i], B[i]);
			}

			SG_Free(R); SG_Free(G); SG_Free(B);

			return( true );
		}
	}

	//-----------------------------------------------------
	return( false );
}

//---------------------------------------------------------
bool CSG_Colors::Save(const CSG_String &File_Name, bool bBinary)
{
	CSG_File	Stream;

	if( Stream.Open(File_Name, SG_FILE_W, bBinary) )
	{
		if( bBinary )
		{
			Stream.Write((void *)COLORS_SERIAL_VERSION_BINARY, sizeof(COLORS_SERIAL_VERSION_BINARY));
		}
		else
		{
			Stream.Write(COLORS_SERIAL_VERSION__ASCII); Stream.Write("\n");
		}

		Serialize(Stream, true, bBinary);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Colors::Serialize(CSG_File &Stream, bool bSave, bool bBinary)
{
	if( Stream.is_Open() )
	{
		//-------------------------------------------------
		if( bBinary )
		{
			if( bSave )
			{
				if( m_nColors > 0 )
				{
					Stream.Write(&m_nColors, sizeof(m_nColors));
					Stream.Write(m_Colors, sizeof(long), m_nColors);
				}
			}
			else
			{
				int	nColors;

				Stream.Read(&nColors, sizeof(m_nColors));

				if( Set_Count(nColors) )
				{
					Stream.Read(m_Colors, sizeof(long), m_nColors);
				}
			}

			return( true );
		}

		//-------------------------------------------------
		else
		{
			if( bSave )
			{
				if( Get_Count() > 0 )
				{
					Stream.Printf("%d\n", Get_Count());

					for(int i=0; i<Get_Count(); i++)
					{
						Stream.Printf("%03d %03d %03d\n", (int)Get_Red(i), (int)Get_Green(i), (int)Get_Blue(i));
					}
				}
			}
			else
			{
				CSG_String	sLine;

				while( Stream.Read_Line(sLine) && sLine.is_Empty() ) {}	// skip empty lines

				if( Set_Count(sLine.asInt()) )
				{
					for(int i=0; i<Get_Count(); i++)
					{
						Stream.Read_Line(sLine);

						Set_Color(i,
							sLine                .asInt(),
							sLine.AfterFirst(' ').asInt(),
							sLine.AfterLast (' ').asInt()
						);
					}
				}
			}

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Colors::to_Text(CSG_String &String)
{
	if( Get_Count() > 0 )
	{
		String.Clear();

		for(int i=0; i<Get_Count(); i++)
		{
			String	+= CSG_String::Format("%03d %03d %03d;", (int)Get_Red(i), (int)Get_Green(i), (int)Get_Blue(i));
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Colors::from_Text(const CSG_String &String)
{
	if( Set_Count((int)String.Length() / 12) )
	{
		for(int i=0, j=0; i<Get_Count(); i++, j+=12)
		{
			Set_Color(i,
				String.Mid(j + 0, 4).asInt(),
				String.Mid(j + 4, 4).asInt(),
				String.Mid(j + 8, 4).asInt()
			);
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
