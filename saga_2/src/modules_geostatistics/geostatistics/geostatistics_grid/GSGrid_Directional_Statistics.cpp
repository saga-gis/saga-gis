
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                  Geostatistics_Grid                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//            GSGrid_Directional_Statistics.cpp          //
//                                                       //
//                 Copyright (C) 2010 by                 //
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
#include "GSGrid_Directional_Statistics.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGSGrid_Directional_Statistics::CGSGrid_Directional_Statistics(void)
{
	Set_Name		(_TL("Directional Statistics for Single Grid"));

	Set_Author		(SG_T("O.Conrad (c) 2010"));

	Set_Description	(_TW(
		"Calculates for each cell statistical properties (arithmetic mean, minimum, maximum, "
		"variance, standard deviation) of all cells lying in given direction based on the input grid. "
	));


	Parameters.Add_Grid(NULL, "GRID"		, _TL("Grid")								, _TL(""), PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, "MEAN"		, _TL("Arithmetic Mean")					, _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid(NULL, "DIFMEAN"		, _TL("Difference from Arithmetic Mean")	, _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid(NULL, "MIN"			, _TL("Minimum")							, _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid(NULL, "MAX"			, _TL("Maximum")							, _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid(NULL, "RANGE"		, _TL("Range")								, _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid(NULL, "VAR"			, _TL("Variance")							, _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid(NULL, "STDDEV"		, _TL("Standard Deviation")					, _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid(NULL, "STDDEVLO"	, _TL("Mean less Standard Deviation")		, _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid(NULL, "STDDEVHI"	, _TL("Mean plus Standard Deviation")		, _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid(NULL, "DEVMEAN"		, _TL("Deviation from Arithmetic Mean")		, _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid(NULL, "PERCENT"		, _TL("Percentile")							, _TL(""), PARAMETER_OUTPUT_OPTIONAL);

	Parameters.Add_Value(
		NULL, "DIRECTION"	, _TL("Direction [Degree]"),
		_TL(""),
		PARAMETER_TYPE_Double
	);

	Parameters.Add_Value(
		NULL, "TOLERANCE"	, _TL("Tolerance [Degree]"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.0, 0.0, true, 45.0, true
	);

	Parameters.Add_Value(
		NULL, "MAXDISTANCE"	, _TL("Maximum Distance [Cells]"),
		_TL("Maximum distance parameter is ignored if set to zero (default)."),
		PARAMETER_TYPE_Int, 0.0, 0.0, true
	);

	Parameters.Add_Parameters(
		NULL, "WEIGHTING"	, _TL("Weighting"),
		_TL("")
	)->asParameters()->Assign(m_Cells.Get_Weighting().Get_Parameters());
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGSGrid_Directional_Statistics::On_Execute(void)
{
	int			Radius;
	double		Direction, Tolerance;

	//-----------------------------------------------------
	m_pGrid		= Parameters("GRID")		->asGrid();

	m_pMean		= Parameters("MEAN")		->asGrid();
	m_pDifMean	= Parameters("DIFMEAN")		->asGrid();
	m_pMin		= Parameters("MIN")			->asGrid();
	m_pMax		= Parameters("MAX")			->asGrid();
	m_pRange	= Parameters("RANGE")		->asGrid();
	m_pVar		= Parameters("VAR")			->asGrid();
	m_pStdDev	= Parameters("STDDEV")		->asGrid();
	m_pStdDevLo	= Parameters("STDDEVLO")	->asGrid();
	m_pStdDevHi	= Parameters("STDDEVHI")	->asGrid();
	m_pDevMean	= Parameters("DEVMEAN")		->asGrid();
	m_pPercent	= Parameters("PERCENT")		->asGrid();

	//-----------------------------------------------------
	if( !m_pGrid || (!m_pMean && !m_pDifMean && !m_pMin && !m_pMax && !m_pRange && !m_pVar && !m_pStdDev && !m_pStdDevLo && !m_pStdDevHi && !m_pPercent) )
	{
		Message_Dlg(_TL("no target grids specified"));

		return( false );
	}

	//-----------------------------------------------------
	Direction	= Parameters("DIRECTION")	->asDouble() * M_DEG_TO_RAD;
	Tolerance	= Parameters("TOLERANCE")	->asDouble() * M_DEG_TO_RAD;

	Radius		= Parameters("MAXDISTANCE")	->asInt();

	if( Radius <= 0 )
	{
		Radius	= 1 + (int)SG_Get_Length(Get_NX(), Get_NY());
	}

	m_Cells.Get_Weighting().Set_Parameters(Parameters("WEIGHTING")->asParameters());

	if( !m_Cells.Set_Sector(Radius, Direction, Tolerance) )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			Get_Statistics(x, y);
		}
	}

	//-----------------------------------------------------
	m_Cells.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGSGrid_Directional_Statistics::Get_Statistics(int x, int y)
{
	if( m_pGrid->is_InGrid(x, y) )
	{
		int		i, ix, iy, nLower;
		double	z, iz, id, iw;

		CSG_Simple_Statistics	Statistics;

		for(i=0, nLower=0, z=m_pGrid->asDouble(x, y); i<m_Cells.Get_Count(); i++)
		{
			if( m_Cells.Get_Values(i, ix = x, iy = y, id, iw, true) && id > 0.0 && m_pGrid->is_InGrid(ix, iy) )
			{
				Statistics.Add_Value(iz = m_pGrid->asDouble(ix, iy), iw);

				if( z > iz )
				{
					nLower++;
				}
			}
		}

		//-------------------------------------------------
		if( Statistics.Get_Weights() > 0.0 )
		{
			if( m_pMean     )	m_pMean		->Set_Value(x, y, Statistics.Get_Mean());
			if( m_pDifMean  )	m_pDifMean	->Set_Value(x, y, m_pGrid->asDouble(x, y) - Statistics.Get_Mean());
			if( m_pMin      )	m_pMin		->Set_Value(x, y, Statistics.Get_Minimum());
			if( m_pMax      )	m_pMax		->Set_Value(x, y, Statistics.Get_Maximum());
			if( m_pRange    )	m_pRange	->Set_Value(x, y, Statistics.Get_Range());
			if( m_pVar      )	m_pVar		->Set_Value(x, y, Statistics.Get_Variance());
			if( m_pStdDev   )	m_pStdDev	->Set_Value(x, y, Statistics.Get_StdDev());
			if( m_pStdDevLo )	m_pStdDevLo	->Set_Value(x, y, Statistics.Get_Mean() - Statistics.Get_StdDev());
			if( m_pStdDevHi )	m_pStdDevHi	->Set_Value(x, y, Statistics.Get_Mean() + Statistics.Get_StdDev());
			if( m_pDevMean  )	m_pDevMean	->Set_Value(x, y, Statistics.Get_StdDev() > 0.0 ? ((z - Statistics.Get_Mean()) / Statistics.Get_StdDev()) : 0.0);
			if( m_pPercent  )	m_pPercent	->Set_Value(x, y, 100.0 * nLower / (double)Statistics.Get_Count());

			return( true );
		}
	}

	//-----------------------------------------------------
	if( m_pMean     )	m_pMean		->Set_NoData(x, y);
	if( m_pDifMean  )	m_pDifMean	->Set_NoData(x, y);
	if( m_pMin      )	m_pMin		->Set_NoData(x, y);
	if( m_pMax      )	m_pMax		->Set_NoData(x, y);
	if( m_pRange    )	m_pRange	->Set_NoData(x, y);
	if( m_pVar      )	m_pVar		->Set_NoData(x, y);
	if( m_pStdDev   )	m_pStdDev	->Set_NoData(x, y);
	if( m_pStdDevLo )	m_pStdDevLo	->Set_NoData(x, y);
	if( m_pStdDevHi )	m_pStdDevHi	->Set_NoData(x, y);
	if( m_pDevMean  )	m_pDevMean	->Set_NoData(x, y);
	if( m_pPercent  )	m_pPercent	->Set_NoData(x, y);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
