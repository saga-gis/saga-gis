
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    statistics_grid                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  GSGrid_Residuals.cpp                 //
//                                                       //
//                 Copyright (C) 2003 by                 //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "GSGrid_Residuals.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGSGrid_Residuals::CGSGrid_Residuals(void)
{
	Set_Name		(_TL("Focal Statistics"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description	(_TW(
		"Based on its neighbourhood this tool calculates for each grid cell "
		"various statistical measures. "
		"According to Wilson & Gallant (2000) this tool was named "
		"'Residual Analysis (Grid)' in earlier versions. "
	));

	Add_Reference(
		"Wilson, J.P. & Gallant, J.C. [Eds.]", "2000",
		"Terrain analysis - principles and applications",
		"New York, John Wiley & Sons, Inc."
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("", "GRID", _TL("Grid"), _TL(""), PARAMETER_INPUT);

	for(int i=0; i<COUNT; i++)
	{
		Parameters.Add_Grid("", Results[i][0], Results[i][1], _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	}

	Parameters.Add_Bool("",
		"BCENTER"	, _TL("Include Center Cell"),
		_TL(""),
		true
	);

	CSG_Grid_Cell_Addressor::Add_Parameters(Parameters, "",
		SG_GRIDCELLADDR_PARM_SQUARE|SG_GRIDCELLADDR_PARM_CIRCLE|SG_GRIDCELLADDR_PARM_ANNULUS|SG_GRIDCELLADDR_PARM_SECTOR
	);

	m_Kernel.Get_Weighting().Set_BandWidth(75.);	// 75%
	m_Kernel.Get_Weighting().Create_Parameters(&Parameters, false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGSGrid_Residuals::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Kernel.Enable_Parameters(*pParameters);

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGSGrid_Residuals::On_Execute(void)
{
	m_pGrid		= Parameters("GRID")->asGrid();

	int	nResults	= 0;

	for(int i=0; i<COUNT; i++)
	{
		if( (m_pResult[i] = Parameters(Results[i][0])->asGrid()) != NULL )
		{
			nResults++;

			m_pResult[i]->Fmt_Name("%s [%s]", m_pGrid->Get_Name(), Results[i][1].c_str());
		}
	}

	if( nResults < 1 )
	{
		Error_Set(_TL("no result has been selected"));

		return( false );
	}

	//-----------------------------------------------------
	if( !m_Kernel.Set_Parameters(Parameters) )
	{
		Error_Set(_TL("could not initialize kernel"));

		return( false );
	}

	m_Kernel.Get_Weighting().Set_BandWidth(m_Kernel.Get_Radius() * m_Kernel.Get_Weighting().Get_BandWidth());

	bool	bCenter	= Parameters("BCENTER")->asBool();

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			Get_Statistics(x, y, bCenter);
		}
	}

	//-----------------------------------------------------
	m_Kernel.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGSGrid_Residuals::Get_Statistics(int x, int y, bool bCenter)
{
	if( m_pGrid->is_InGrid(x, y) )
	{
		int		i, ix, iy, nLower;
		double	z, iz, id, iw;

		CSG_Simple_Statistics	s(m_pResult[MEDIAN] != NULL);

		CSG_Unique_Number_Statistics	u(m_Kernel.Get_Weighting().Get_Weighting() != SG_DISTWGHT_None);

		for(i=0, nLower=0, z=m_pGrid->asDouble(x, y); i<m_Kernel.Get_Count(); i++)
		{
			if( m_Kernel.Get_Values(i, ix = x, iy = y, id, iw, true) && (bCenter || id > 0.) && m_pGrid->is_InGrid(ix, iy) )
			{
				double	iz	= m_pGrid->asDouble(ix, iy);

				s.Add_Value(iz, iw);

				if( z > iz )
				{
					nLower++;
				}

				if( m_pResult[MINORITY] || m_pResult[MAJORITY] )
				{
					u.Add_Value(iz, iw);
				}
			}
		}

		//-------------------------------------------------
		if( s.Get_Weights() > 0. )
		{
			#define SET_VALUE(key, value)	if( m_pResult[key] ) { m_pResult[key]->Set_Value(x, y, value); }

			SET_VALUE(MEAN    , s.Get_Mean    ());
			SET_VALUE(MEDIAN  , s.Get_Median  ());
			SET_VALUE(MIN     , s.Get_Minimum ());
			SET_VALUE(MAX     , s.Get_Maximum ());
			SET_VALUE(RANGE   , s.Get_Range   ());
			SET_VALUE(STDDEV  , s.Get_StdDev  ());
			SET_VALUE(VARIANCE, s.Get_Variance());
			SET_VALUE(SUM     , s.Get_Sum     ());
			SET_VALUE(DIFF    , z - s.Get_Mean());
			SET_VALUE(DEVMEAN , s.Get_StdDev() <= 0. ? 0. : (z - s.Get_Mean()) / s.Get_StdDev());
			SET_VALUE(PERCENT , nLower * 100. / s.Get_Count());
		}

		if( u.Get_Count() > 0. )
		{
			if( m_pResult[MINORITY] && u.Get_Minority(z) ) { m_pResult[MINORITY]->Set_Value(x, y, z); }
			if( m_pResult[MAJORITY] && u.Get_Majority(z) ) { m_pResult[MAJORITY]->Set_Value(x, y, z); }
		}

		return( true );
	}

	//-----------------------------------------------------
	for(int i=0; i<COUNT; i++)
	{
		if( m_pResult[i] )
		{
			m_pResult[i]->Set_NoData(x, y);
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
