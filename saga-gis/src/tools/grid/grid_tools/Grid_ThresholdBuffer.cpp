
/*******************************************************************************
    ThresholdBuffer.cpp
    Copyright (C) Victor Olaya

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, USA
*******************************************************************************/

//---------------------------------------------------------
#include "Grid_ThresholdBuffer.h"

//---------------------------------------------------------
#define BUFFER  1
#define FEATURE 2
#define THRESHOLD_ABSOLUTE 0
#define THRESHOLD_RELATIVE 1


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CThresholdBuffer::CThresholdBuffer(void)
{
	Set_Name		(_TL("Threshold Buffer"));

	Set_Author		("Victor Olaya (c) 2004");

	Set_Description(_TW(
		"Threshold Buffer Creation"
	));

	Parameters.Add_Grid  ("", "FEATURES"     , _TL("Features"       ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid  ("", "VALUE"        , _TL("Value"          ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid  ("", "THRESHOLDGRID", _TL("Threshold"      ), _TL(""), PARAMETER_INPUT_OPTIONAL);
	Parameters.Add_Grid  ("", "BUFFER"       , _TL("Buffer"         ), _TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Int);
	Parameters.Add_Double("", "THRESHOLD"    , _TL("Threshold Value"), _TL(""));
	Parameters.Add_Choice("", "THRESHOLDTYPE", _TL("Threshold Type" ), _TL(""), CSG_String::Format("%s|%s",
		_TL("Absolute"),
		_TL("Relative from cell value"))
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CThresholdBuffer::On_Execute(void)
{
	m_pFeatures  = Parameters("FEATURES"     )->asGrid  ();
	m_pValues    = Parameters("VALUE"        )->asGrid  ();
	m_pBuffer    = Parameters("BUFFER"       )->asGrid  ();
	m_pThreshold = Parameters("THRESHOLDGRID")->asGrid  ();
	m_Threshold  = Parameters("THRESHOLD"    )->asDouble();
	m_Type       = Parameters("THRESHOLDTYPE")->asInt   ();

	m_pBuffer->Assign(0.);

	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_pFeatures->is_NoData(x, y) && m_pFeatures->asDouble(x, y) != 0. )
			{
				BufferPoint(x, y);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CThresholdBuffer::BufferPoint(int x, int y)
{
	double BaseValue = m_pValues->asDouble(x, y);

	double Threshold = m_pThreshold ? m_pThreshold->asDouble(x, y) : m_Threshold;

	CSG_Points_Int Centrals, Adjacents;

	Centrals.Add(x, y);

	m_pBuffer->Set_Value(x, y, FEATURE);

	while( Centrals.Get_Count() != 0 )
	{
		for(int iPt=0; iPt<Centrals.Get_Count();iPt++)
		{
			x = Centrals[iPt].x;
			y = Centrals[iPt].y;

			if( !m_pValues->is_NoData(x,y) )
			{
				for(int i=0; i<8; i++)
				{
					int ix = Get_xTo(i, x);
					int iy = Get_yTo(i, y);

					if( m_pValues->is_InGrid(ix, iy) && m_pBuffer->asInt(ix, iy) == 0 )
					{
						double Value = m_Type == THRESHOLD_ABSOLUTE
							? m_pValues->asDouble(ix, iy)
							: fabs(m_pValues->asDouble(ix, iy) - BaseValue);

						if( Value < Threshold )
						{
							m_pBuffer->Set_Value(ix, iy, BUFFER);
							Adjacents.Add(ix, iy);
						}
					}
				}
			}
		}

		Centrals.Clear();

		for(int iPt=0; iPt<Adjacents.Get_Count(); iPt++)
		{
			x = Adjacents[iPt].x;
			y = Adjacents[iPt].y;

			Centrals.Add(x, y);
		}

		Adjacents.Clear();
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
