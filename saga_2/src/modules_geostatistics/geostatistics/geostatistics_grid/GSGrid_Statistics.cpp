
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

	Set_Author		(_TL("Copyrights (c) 2005 by Olaf Conrad"));

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
	int						x, y, i, n;
	double					z, m, v, min, max;
	CSG_Grid					*pMean, *pMin, *pMax, *pVar, *pStdDev, *pStdDevLo, *pStdDevHi;
	CSG_Parameter_Grid_List	*pGrids;

	//-----------------------------------------------------
	pGrids		= Parameters("GRIDS")	->asGridList();

	pMean		= Parameters("MEAN")	->asGrid();
	pMin		= Parameters("MIN")		->asGrid();
	pMax		= Parameters("MAX")		->asGrid();
	pVar		= Parameters("VAR")		->asGrid();
	pStdDev		= Parameters("STDDEV")	->asGrid();
	pStdDevLo	= Parameters("STDDEVLO")->asGrid();
	pStdDevHi	= Parameters("STDDEVHI")->asGrid();

	//-----------------------------------------------------
	if( pGrids->Get_Count() > 1 && (pMean || pMin || pMax || pVar || pStdDev || pStdDevLo || pStdDevHi) )
	{
		for(y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				for(i=0, n=0, m=0.0, v=0.0; i<pGrids->Get_Count(); i++)
				{
					if( !pGrids->asGrid(i)->is_NoData(x, y) )
					{
						z	= pGrids->asGrid(i)->asDouble(x, y);

						if( n == 0 )
						{
							min	= max	= z;
						}
						else if( min > z )
						{
							min	= z;
						}
						else if( max < z )
						{
							max	= z;
						}

						m	+= z;
						v	+= z * z;
						n++;
					}
				}

				//-----------------------------------------
				if( n == 0 )
				{
					if( pMean )		pMean		->Set_NoData(x, y);
					if( pMin )		pMin		->Set_NoData(x, y);
					if( pMax )		pMax		->Set_NoData(x, y);
					if( pVar )		pVar		->Set_NoData(x, y);
					if( pStdDev )	pStdDev		->Set_NoData(x, y);
					if( pStdDevLo )	pStdDevLo	->Set_NoData(x, y);
					if( pStdDevHi )	pStdDevHi	->Set_NoData(x, y);
				}
				else
				{
					m	= m / (double)n;
					v	= v / (double)n - m * m;
					z	= sqrt(v);

					if( pMean )		pMean		->Set_Value(x, y, m);
					if( pMin )		pMin		->Set_Value(x, y, min);
					if( pMax )		pMax		->Set_Value(x, y, max);
					if( pVar )		pVar		->Set_Value(x, y, v);
					if( pStdDev )	pStdDev		->Set_Value(x, y, z);
					if( pStdDevLo )	pStdDevLo	->Set_Value(x, y, m - z);
					if( pStdDevHi )	pStdDevHi	->Set_Value(x, y, m + z);
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
