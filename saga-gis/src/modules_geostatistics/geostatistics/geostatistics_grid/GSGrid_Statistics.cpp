/**********************************************************
 * Version $Id$
 *********************************************************/

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
//                 GSGrid_Statistics.cpp                 //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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
#include "GSGrid_Statistics.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGSGrid_Statistics::CGSGrid_Statistics(void)
{
	Set_Name		(_TL("Statistics for Grids"));

	Set_Author		(SG_T("O.Conrad (c) 2005"));

	Set_Description	(_TW("Calculates statistical properties (arithmetic mean, minimum, maximum, "
		"variance, standard deviation) for each cell position for the values of "
		"the selected grids."
	));


	Parameters.Add_Grid_List(
		NULL, "GRIDS"	, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "MEAN"	, _TL("Arithmetic Mean"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL, "MIN"		, _TL("Minimum"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL, "MAX"		, _TL("Maximum"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL, "VAR"		, _TL("Variance"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL, "STDDEV"	, _TL("Standard Deviation"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL, "STDDEVLO", _TL("Mean less Standard Deviation"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL, "STDDEVHI", _TL("Mean plus Standard Deviation"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL, "RANK"	, _TL("Percentile"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Value(
		NULL, "RANK_VAL", _TL("Percentile"),
		_TL(""),
		PARAMETER_TYPE_Double, 50.0, 0.0, true, 100.0, true
	);
}

//---------------------------------------------------------
CGSGrid_Statistics::~CGSGrid_Statistics(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGSGrid_Statistics::On_Execute(void)
{
	double					dRank;
	CSG_Grid				*pMean, *pMin, *pMax, *pVar, *pStdDev, *pStdDevLo, *pStdDevHi, *pPercentile;
	CSG_Parameter_Grid_List	*pGrids;

	//-----------------------------------------------------
	pGrids		= Parameters("GRIDS"   )->asGridList();

	pMean		= Parameters("MEAN"    )->asGrid();
	pMin		= Parameters("MIN"     )->asGrid();
	pMax		= Parameters("MAX"     )->asGrid();
	pVar		= Parameters("VAR"     )->asGrid();
	pStdDev		= Parameters("STDDEV"  )->asGrid();
	pStdDevLo	= Parameters("STDDEVLO")->asGrid();
	pStdDevHi	= Parameters("STDDEVHI")->asGrid();
	pPercentile	= Parameters("RANK"    )->asGrid();

	dRank		= Parameters("RANK_VAL")->asDouble() / 100.0;

	//-----------------------------------------------------
	if( pGrids->Get_Count() > 1 && (pMean || pMin || pMax || pVar || pStdDev || pStdDevLo || pStdDevHi || pPercentile) )
	{
		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			#pragma omp parallel for
			for(int x=0; x<Get_NX(); x++)
			{
				CSG_Table				Values;
				CSG_Simple_Statistics	s;

				for(int i=0; i<pGrids->Get_Count(); i++)
				{
					if( !pGrids->asGrid(i)->is_NoData(x, y) )
					{
						double	z	= pGrids->asGrid(i)->asDouble(x, y);

						s.Add_Value(z);

						if( pPercentile )
						{
							if( Values.Get_Field_Count() == 0 )
							{
								Values.Add_Field("Z", SG_DATATYPE_Double);
							}

							Values.Add_Record()->Set_Value(0, z);
						}
					}
				}

				//-----------------------------------------
				if( s.Get_Count() <= 0 )
				{
					if( pMean       )	pMean		->Set_NoData(x, y);
					if( pMin        )	pMin		->Set_NoData(x, y);
					if( pMax        )	pMax		->Set_NoData(x, y);
					if( pVar        )	pVar		->Set_NoData(x, y);
					if( pStdDev     )	pStdDev		->Set_NoData(x, y);
					if( pStdDevLo   )	pStdDevLo	->Set_NoData(x, y);
					if( pStdDevHi   )	pStdDevHi	->Set_NoData(x, y);
					if( pPercentile )	pPercentile	->Set_NoData(x, y);
				}
				else
				{
					if( pMean       )	pMean		->Set_Value(x, y, s.Get_Mean());
					if( pMin        )	pMin		->Set_Value(x, y, s.Get_Minimum());
					if( pMax        )	pMax		->Set_Value(x, y, s.Get_Maximum());
					if( pVar        )	pVar		->Set_Value(x, y, s.Get_Variance());
					if( pStdDev     )	pStdDev		->Set_Value(x, y, s.Get_StdDev());
					if( pStdDevLo   )	pStdDevLo	->Set_Value(x, y, s.Get_Mean() - s.Get_StdDev());
					if( pStdDevHi   )	pStdDevHi	->Set_Value(x, y, s.Get_Mean() + s.Get_StdDev());
					if( pPercentile )
					{
						Values.Set_Index(0, TABLE_INDEX_Ascending);

						pPercentile->Set_Value(x, y, Values.Get_Record_byIndex((int)(dRank * s.Get_Count()))->asDouble(0));
					}
				}
			}
		}

		return( true );
	}

	//-----------------------------------------------------
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
