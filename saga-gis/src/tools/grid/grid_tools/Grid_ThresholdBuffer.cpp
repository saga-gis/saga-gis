
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
		"This tool performs a feature-buffering operation on a raster layer with additional constraint logic based on a secondary raster input. "
		"Unlike standard buffers, it dynamically extends the buffer zone by evaluating each neighboring cell against a user-defined threshold. "
		"The extension is conditionally applied based on the values in the secondary raster and can operate in two modes:\n"
		"- Absolute: compares each neighboring cell’s value in the secondary raster directly to the threshold.\n"
		"- Relative to start cell value: compares each neighboring cell’s value relative to the starting cell’s value.\n"
		"Buffer expansion occurs only if the increase or change is within the threshold, allowing for value-sensitive spatial analysis, such "
		"as terrain-aware or cost-constrained buffering."
	));

	Parameters.Add_Grid  ("", "FEATURES"     , _TL("Features"       ), _TL("Input grid with the features to buffer where valid grid cells are all non-zero, non-NoData values. Cells with a value of 0 or NoData are ignored."), PARAMETER_INPUT);
	Parameters.Add_Grid  ("", "VALUE"        , _TL("Value"          ), _TL("Secondary input grid with the values used to evaluate the threshold value."), PARAMETER_INPUT);
	Parameters.Add_Grid  ("", "THRESHOLDGRID", _TL("Threshold"      ), _TL("Optional input grid with spatially variable threshold values."), PARAMETER_INPUT_OPTIONAL);
	Parameters.Add_Grid  ("", "BUFFER"       , _TL("Buffer"         ), _TL("The buffered output grid. Input features are labelled by 2, buffer zones by 1 and all other cells with 0."), PARAMETER_OUTPUT, true, SG_DATATYPE_Int);
	Parameters.Add_Double("", "THRESHOLD"    , _TL("Threshold Value"), _TL("The threshold value to apply. Expansion occurs only if the increase or change is within the threshold."));
	Parameters.Add_Choice("", "THRESHOLDTYPE", _TL("Threshold Type" ), _TL("The mode of operation."), CSG_String::Format("%s|%s",
		_TL("Absolute"),
		_TL("Relative (to start cell value)"))
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
