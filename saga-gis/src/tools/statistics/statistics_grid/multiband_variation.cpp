/**********************************************************
 * Version $Id: multiband_variation.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

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
//                multiband_variation.cpp                //
//                                                       //
//                 Copyright (C) 2011 by                 //
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
#include "multiband_variation.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CMultiBand_Variation::CMultiBand_Variation(void)
{
	Set_Name		(_TL("Multi-Band Variation"));

	Set_Author		("O.Conrad (c) 2010");

	Set_Description	(_TW(
		"Calculates for each cell the spectral variation based on feature space distances "
		"to the centroid for all cells in specified neighbourhood. "
		"The average distance has been used for Spectral Variation Hypothesis (SVH)."
	));

	Add_Reference(
		"Palmer, M.W., Earls, P., Hoagland, B.W., White, P.S., Wohlgemuth, T.", "2002",
		"Quantitative tools for perfecting species lists",
		"Environmetrics 13, 121–137."
	);

	Parameters.Add_Grid_List(
		"", "BANDS"		, _TL("Bands"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "MEAN"		, _TL("Mean Distance"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		"", "STDDEV"	, _TL("Standard Deviation"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		"", "DIFF"		, _TL("Distance"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Int(
		"", "RADIUS"	, _TL("Radius [Cells]"),
		_TL(""),
		1, 1, true
	);

	m_Cells.Get_Weighting().Create_Parameters(&Parameters, false);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CMultiBand_Variation::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( m_Cells.Get_Weighting().Enable_Parameters(pParameters) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CMultiBand_Variation::On_Execute(void)
{
	//-----------------------------------------------------
	m_pBands	= Parameters("BANDS" )->asGridList();
	m_pMean		= Parameters("MEAN"  )->asGrid();
	m_pStdDev	= Parameters("STDDEV")->asGrid();
	m_pDiff		= Parameters("DIFF"  )->asGrid();

	//-----------------------------------------------------
	if( m_pBands->Get_Grid_Count() < 1 )
	{
		Error_Set(_TL("no input"));

		return( false );
	}

	//-----------------------------------------------------
	m_Cells.Get_Weighting().Set_Parameters(&Parameters);

	if( !m_Cells.Set_Radius(Parameters("RADIUS")->asInt()) )
	{
		Error_Set(_TL("failed to set radius"));

		return( false );
	}

	//-----------------------------------------------------
	int		x, y;

	m_Mask.Create(*Get_System(), SG_DATATYPE_Byte);
	m_Mask.Set_NoData_Value(0);

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for private(x)
		for(x=0; x<Get_NX(); x++)
		{
			bool	bNoData	= false;

			for(int iBand=0; iBand<m_pBands->Get_Grid_Count() && !bNoData; iBand++)
			{
				if( m_pBands->Get_Grid(iBand)->is_NoData(x, y) )
				{
					bNoData	= true;
				}
			}

			m_Mask.Set_Value(x, y, bNoData ? 0 : 1);
		}
	}

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for private(x)
		for(x=0; x<Get_NX(); x++)
		{
			Get_Variation(x, y);
		}
	}

	//-----------------------------------------------------
	m_Mask .Destroy();
	m_Cells.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CMultiBand_Variation::Get_Variation(int x, int y)
{
	if( !m_Mask.is_NoData(x, y) )
	{
		int			iBand, iCell, ix, iy;
		double		iDistance, iWeight, Weights, Distance;
		CSG_Vector	Centroid(m_pBands->Get_Grid_Count());

		//-------------------------------------------------
		for(iCell=0, Weights=0.0; iCell<m_Cells.Get_Count(); iCell++)
		{
			if( m_Cells.Get_Values(iCell, ix = x, iy = y, iDistance, iWeight, true) && m_Mask.is_InGrid(ix, iy) )
			{
				for(iBand=0; iBand<m_pBands->Get_Grid_Count(); iBand++)
				{
					Centroid[iBand]	+= iWeight * m_pBands->Get_Grid(iBand)->asDouble(ix, iy);
				}

				Weights			+= iWeight;
			}
		}

		//-------------------------------------------------
		if( Weights > 0.0 )
		{
			CSG_Simple_Statistics	s;

			Centroid	*= 1.0 / Weights;

			for(iCell=0; iCell<m_Cells.Get_Count(); iCell++)
			{
				if( m_Cells.Get_Values(iCell, ix = x, iy = y, iDistance, iWeight, true) && m_Mask.is_InGrid(ix, iy) )
				{
					for(iBand=0, Distance=0.0; iBand<m_pBands->Get_Grid_Count(); iBand++)
					{
						Distance	+= SG_Get_Square(Centroid[iBand] - m_pBands->Get_Grid(iBand)->asDouble(ix, iy));
					}

					s.Add_Value(sqrt(Distance), iWeight);

					if( ix == x && iy == y )
					{
						if( m_pDiff )	m_pDiff->Set_Value(x, y, sqrt(Distance));
					}
				}
			}

			if( m_pMean   )	m_pMean  ->Set_Value(x, y, s.Get_Mean());
			if( m_pStdDev )	m_pStdDev->Set_Value(x, y, s.Get_StdDev());

			return( true );
		}
	}

	//-----------------------------------------------------
	if( m_pMean   )	m_pMean  ->Set_NoData(x, y);
	if( m_pStdDev )	m_pStdDev->Set_NoData(x, y);
	if( m_pDiff   )	m_pDiff  ->Set_NoData(x, y);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
