
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
//                   sgdi_controls.cpp                   //
//                                                       //
//                 Copyright (C) 2020 by                 //
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
#include <wx/wxprec.h>
#include <wx/dc.h>

//---------------------------------------------------------
#include "sgdi_controls.h"


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

	SetTickFreq(SLIDER_RANGE / 10);
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
		minValue	= 0.;
		maxValue	= 1.;
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
		minValue	= 0.;
		maxValue	= 1.;
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
