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
//                  Library: SAGA_GDI                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    sgdi_helper.cpp                    //
//                                                       //
//                 Copyright (C) 2008 by                 //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Bundesstr. 55                          //
//                20146 Hamburg                          //
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
#include <wx/wxprec.h>

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#include <wx/dc.h>

//---------------------------------------------------------
#include "sgdi_helper.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef M_PI
#define M_PI						3.141592653589793
#endif

#ifndef M_DEG_TO_RAD
#define M_DEG_TO_RAD				(M_PI / 180.0)
#endif


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void		Draw_Text(wxDC &dc, int Align, int x, int y, const wxString &Text)
{
	wxCoord	xSize, ySize;

	if( Align != TEXTALIGN_TOPLEFT )
	{
		dc.GetTextExtent(Text, &xSize, &ySize);

		//-------------------------------------------------
		if		( Align & TEXTALIGN_XCENTER )
		{
			x	-= xSize / 2;
		}
		else if	( Align & TEXTALIGN_RIGHT )
		{
			x	-= xSize;
		}

		//-------------------------------------------------
		if		( Align & TEXTALIGN_YCENTER )
		{
			y	-= ySize / 2;
		}
		else if	( Align & TEXTALIGN_BOTTOM )
		{
			y	-= ySize;
		}
	}

	dc.DrawText(Text, x, y);
}

//---------------------------------------------------------
void		Draw_Text(wxDC &dc, int Align, int x, int y, double Angle, const wxString &Text)
{
	double	d;
	wxCoord	xSize, ySize;

	if( Align != TEXTALIGN_TOPLEFT )
	{
		dc.GetTextExtent(Text, &xSize, &ySize);

		//-------------------------------------------------
		d	 = M_DEG_TO_RAD * Angle;

		if		( Align & TEXTALIGN_XCENTER )
		{
			x	-= (int)(xSize * cos(d) / 2.0);
			y	+= (int)(xSize * sin(d) / 2.0);
		}
		else if	( Align & TEXTALIGN_RIGHT )
		{
			x	-= (int)(xSize * cos(d));
			y	+= (int)(xSize * sin(d));
		}

		//-------------------------------------------------
		d	 = M_DEG_TO_RAD * (Angle - 90.0);

		if		( Align & TEXTALIGN_YCENTER )
		{
			x	-= (int)(ySize * cos(d) / 2.0);
			y	+= (int)(ySize * sin(d) / 2.0);
		}
		else if	( Align & TEXTALIGN_BOTTOM )
		{
			x	-= (int)(ySize * cos(d));
			y	+= (int)(ySize * sin(d));
		}
	}

	dc.DrawRotatedText(Text, x, y, Angle);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define RULER_TEXT_SPACE	4

//---------------------------------------------------------
bool		Draw_Ruler(wxDC &dc, const wxRect &r, bool bHorizontal, double zMin, double zMax, bool bAscendent, int FontSize, const wxColour &Colour)
{
	int			xMin, xMax, yMin, yMax, Decimals, dxFont, dyFont, zPos;
	double		Width, z, dz, zToDC, zDC;
	wxString	s;

	//-----------------------------------------------------
	if( zMin < zMax && r.GetWidth() > 0 && r.GetHeight() > 0 )
	{
		dc.SetPen(wxPen(Colour));
		dc.SetFont(wxFont(7, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

		Width		= bHorizontal ? r.GetWidth() : r.GetHeight();

		xMin		= r.GetX();
		xMax		= r.GetX() + r.GetWidth();
		yMin		= r.GetY() + r.GetHeight();
		yMax		= r.GetY();

		//-------------------------------------------------
		zToDC		= (double)Width / (zMax - zMin);

		dz			= pow(10.0, floor(log10(zMax - zMin)) - 1.0);
		Decimals	= dz >= 1.0 ? 0 : (int)fabs(log10(dz));

		s.Printf(wxT("%.*f"), Decimals, zMax);
		dyFont		= RULER_TEXT_SPACE + dc.GetTextExtent(s).y;
		dxFont		= RULER_TEXT_SPACE;

		zDC			= 2 * dc.GetTextExtent(s).x;
		while( zToDC * dz < zDC + RULER_TEXT_SPACE )
		{
			dz	*= 2;
		}

		//-------------------------------------------------
		z			= dz * floor(zMin / dz);
		if( z < zMin )	z	+= dz;

		for(; z<=zMax; z+=dz)
		{
			s.Printf(wxT("%.*f"), Decimals, z);

			zDC	= bAscendent ? zToDC * (z - zMin) : Width - zToDC * (z - zMin);

			if( bHorizontal )
			{
				zPos	= (int)(xMin + zDC);
				dc.DrawLine(zPos, yMin, zPos, yMax);
				dc.DrawText(s, zPos + dxFont, yMin - dyFont);
			}
			else
			{
				zPos	= (int)(yMin - zDC);
				dc.DrawLine(xMin, zPos, xMax, zPos);
				dc.DrawText(s, xMin + dxFont, zPos - dyFont);
			}
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
#define SLIDER_RANGE	100

//---------------------------------------------------------
CSGDI_Slider::CSGDI_Slider(wxWindow *pParent, int ID, double Value, double minValue, double maxValue, const wxPoint &Point, const wxSize &Size, long Style)
	: wxSlider(pParent, ID, 0, 0, SLIDER_RANGE, Point, Size, Style)
{
	Set_Range(minValue, maxValue);

	Set_Value(Value);

	SetTickFreq(SLIDER_RANGE / 10, 0);
}

//---------------------------------------------------------
CSGDI_Slider::~CSGDI_Slider(void)
{}

//---------------------------------------------------------
bool CSGDI_Slider::Set_Value(double Value)
{
	int		Position	= (int)((double)SLIDER_RANGE * (Value - m_Min) / (m_Max - m_Min));

	if( Position <= 0 )
	{
		SetValue(0);
	}
	else if( Position >= SLIDER_RANGE )
	{
		SetValue(SLIDER_RANGE);
	}
	else
	{
		SetValue(Position);
	}

	return( true );
}

//---------------------------------------------------------
double CSGDI_Slider::Get_Value(void)
{
	return( m_Min + GetValue() * (m_Max - m_Min) / (double)SLIDER_RANGE );
}

//---------------------------------------------------------
bool CSGDI_Slider::Set_Range(double minValue, double maxValue)
{
	if( maxValue == minValue )
	{
		minValue	= 0.0;
		maxValue	= 1.0;
	}

	m_Min	= minValue;
	m_Max	= maxValue;

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSGDI_SpinCtrl::CSGDI_SpinCtrl(wxWindow *pParent, int ID, double Value, double minValue, double maxValue, bool bPercent, const wxPoint &Point, const wxSize &Size, long Style)
	: wxSpinCtrl(pParent, ID, wxEmptyString, Point, Size, Style, bPercent ? 0 : (int)minValue, bPercent ? 100 : (int)maxValue)
{
	m_bPercent	= bPercent;

	Set_Range(minValue, maxValue);

	Set_Value(Value);
}

//---------------------------------------------------------
CSGDI_SpinCtrl::~CSGDI_SpinCtrl(void)
{}

//---------------------------------------------------------
bool CSGDI_SpinCtrl::Set_Value(double Value)
{
	if( m_bPercent )
	{
		int		Position	= (int)((double)SLIDER_RANGE * (Value - m_Min) / (m_Max - m_Min));

		if( Position <= 0 )
		{
			SetValue(0);
		}
		else if( Position >= SLIDER_RANGE )
		{
			SetValue(SLIDER_RANGE);
		}
		else
		{
			SetValue(Position);
		}
	}
	else
	{
		if( Value <= m_Min )
		{
			SetValue((int)m_Min);
		}
		else if( Value >= m_Max )
		{
			SetValue((int)m_Max);
		}
		else
		{
			SetValue((int)Value);
		}
	}

	return( true );
}

//---------------------------------------------------------
double CSGDI_SpinCtrl::Get_Value(void)
{
	if( m_bPercent )
	{
		return( m_Min + GetValue() * (m_Max - m_Min) / (double)SLIDER_RANGE );
	}

	return( GetValue() );
}

//---------------------------------------------------------
bool CSGDI_SpinCtrl::Set_Range(double minValue, double maxValue)
{
	if( maxValue == minValue )
	{
		minValue	= 0.0;
		maxValue	= 1.0;
	}

	m_Min	= minValue;
	m_Max	= maxValue;

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
