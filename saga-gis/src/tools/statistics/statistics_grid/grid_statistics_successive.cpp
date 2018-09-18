
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
//             grid_statistics_successive.cpp            //
//                                                       //
//                Copyrights (C) 2018 by                 //
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
#include "grid_statistics_successive.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Statistics_Build::CGrid_Statistics_Build(void)
{
	Set_Name		(_TL("Build Statistics for Grids"));

	Set_Author		("O.Conrad (c) 2018");

	Set_Description	(_TW(
		"This tool allows to collect successively cell-wise statistical information "
		"from grids by subsequent calls with the 'Reset' flag set to false. The collected "
		"information can be used as input for the 'Evaluate Statistics for Grids' tool. "
		"Together these tools have been designed to inspect a large number of grids "
		"that could not be loaded into memory simultaneously. "
	));

	Parameters.Add_Grid_List("",
		"GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid ("", "COUNT"    , _TL("Number of Values"), _TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Word);
	Parameters.Add_Grid ("", "MIN"      , _TL("Minimum"         ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid ("", "MAX"      , _TL("Maximum"         ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid ("", "SUM"      , _TL("Sum"             ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid ("", "SUM2"     , _TL("Sum of Squares"  ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grids("", "HISTOGRAM", _TL("Histogram"       ), _TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Word);

	Parameters.Add_Bool("", "RESET", _TL("Reset"), _TL(""), true);

	Parameters.Add_Int   ("HISTOGRAM", "HCLASSES", _TL("Number of Classes"), _TL(""), 20, 2, true);
	Parameters.Add_Double("HISTOGRAM", "HMIN"    , _TL("Minimum"          ), _TL(""), 0.0);
	Parameters.Add_Double("HISTOGRAM", "HMAX"    , _TL("Minimum"          ), _TL(""), 0.0);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Statistics_Build::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "RESET") )
	{
		pParameters->Set_Enabled("HCLASSES", pParameter->asBool());
		pParameters->Set_Enabled("HMIN"    , pParameter->asBool());
		pParameters->Set_Enabled("HMAX"    , pParameter->asBool());
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Statistics_Build::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pGrids	= Parameters("GRIDS")->asGridList();

	if( pGrids->Get_Grid_Count() < 1 )
	{
		Error_Set(_TL("no grids in selection"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid	*pCount		= Parameters("COUNT"    )->asGrid ();
	CSG_Grid	*pMin		= Parameters("MIN"      )->asGrid ();
	CSG_Grid	*pMax		= Parameters("MAX"      )->asGrid ();
	CSG_Grid	*pSum		= Parameters("SUM"      )->asGrid ();
	CSG_Grid	*pSum2		= Parameters("SUM2"     )->asGrid ();
	CSG_Grids	*pHistogram	= Parameters("HISTOGRAM")->asGrids();

	double	hLag, hMin, hMax;

	if( Parameters("RESET")->asBool() )
	{
		pCount->Assign(0.0); pCount->Set_NoData_Value(65535);
		pSum  ->Assign(0.0);
		pSum2 ->Assign(0.0);
		pMin  ->Assign_NoData();
		pMax  ->Assign_NoData();

		int	nz	= Parameters("HCLASSES")->asInt();

		if( nz < 2 )
		{
			Error_Set(_TL("histogram needs at least tow classes"));

			return( false );
		}

		hMin	= Parameters("HMIN")->asDouble();
		hMax	= Parameters("HMAX")->asDouble();

		if( hMin >= hMax )
		{
			Error_Set(_TL("histogram's minimum class value needs to be less than its maximum."));

			return( false );
		}

		hLag	= (hMax - hMin) / nz;

		pHistogram->Create(*Get_System(), nz, SG_DATATYPE_Word);
		pHistogram->Set_Name(_TL("Histogram"));

		for(int z=0; z<pHistogram->Get_NZ(); z++)
		{
			pHistogram->Set_Z(z, hMin + (0.5 + z) * hLag);
		}
	}
	else if( pHistogram->Get_NZ() < 2 )
	{
		Error_Set(_TL("histogram needs at least tow classes"));

		return( false );
	}
	else
	{
		hLag	= pHistogram->Get_ZRange() / (pHistogram->Get_NZ() - 1);
		hMin	= pHistogram->Get_ZMin() - 0.5 * hLag;
		hMax	= pHistogram->Get_ZMax() + 0.5 * hLag;
	}

	hLag	= pHistogram->Get_NZ() / (hMax - hMin);

	//-----------------------------------------------------
	for(int	iGrid=0; iGrid<pGrids->Get_Grid_Count() && Set_Progress(iGrid, pGrids->Get_Grid_Count()); iGrid++)
	{
		CSG_Grid	*pGrid	= pGrids->Get_Grid(iGrid);

		#ifndef _DEBUG
		#pragma omp parallel for
		#endif
		for(int y=0; y<Get_NY(); y++) for(int x=0; x<Get_NX(); x++)
		{
			if( pGrid->is_NoData(x, y) )
			{
				if( !pCount->is_NoData(x, y) )
				{
					pCount->Set_NoData(x, y);
					pSum  ->Set_NoData(x, y);
					pSum2 ->Set_NoData(x, y);
					pMin  ->Set_NoData(x, y);
					pMax  ->Set_NoData(x, y);

					for(int z=0; z<pHistogram->Get_NZ(); z++)
					{
						pHistogram->Set_NoData(x, y, z);
					}
				}
			}
			else if( !pCount->is_NoData(x, y) )
			{
				double	Value	= pGrid->asDouble(x, y);

				if( pCount->asInt(x, y) < 1 )
				{
					pCount->Set_Value(x, y, 1.);
					pSum  ->Set_Value(x, y, Value);
					pSum2 ->Set_Value(x, y, Value*Value);
					pMin  ->Set_Value(x, y, Value);
					pMax  ->Set_Value(x, y, Value);
				}
				else
				{
					pCount->Add_Value(x, y, 1.);
					pSum  ->Add_Value(x, y, Value);
					pSum2 ->Add_Value(x, y, Value*Value);

					if( Value < pMin->asDouble(x, y) ) { pMin->Set_Value(x, y, Value); } else
					if( Value > pMax->asDouble(x, y) ) { pMax->Set_Value(x, y, Value); }

					int	z	= (int)(0.5 + (Value - hMin) * hLag);

					if( z >= 0 && z < pHistogram->Get_NZ() )
					{
						pHistogram->Add_Value(x, y, z, 1.);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Statistics_Evaluate::CGrid_Statistics_Evaluate(void)
{
	Set_Name		(_TL("Evaluate Statistics for Grids"));

	Set_Author		("O.Conrad (c) 2018");

	Set_Description	(_TW(
		"Calculates statistical properties (arithmetic mean, range, variance, "
		"standard deviation, percentiles) on a cell-wise base. "
		"This tool takes input about basic statistical information as "
		"it can be collected with the 'Build Statistics for Grids' tool. "
		"Together these tools have been designed to inspect a large number of grids "
		"that could not be loaded into memory simultaneously. "
	));

	Parameters.Add_Grid ("", "COUNT"    , _TL("Number of Values"  ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid ("", "MIN"      , _TL("Minimum"           ), _TL(""), PARAMETER_INPUT_OPTIONAL);
	Parameters.Add_Grid ("", "MAX"      , _TL("Maximum"           ), _TL(""), PARAMETER_INPUT_OPTIONAL);
	Parameters.Add_Grid ("", "SUM"      , _TL("Sum"               ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid ("", "SUM2"     , _TL("Sum of Squares"    ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grids("", "HISTOGRAM", _TL("Histogram"         ), _TL(""), PARAMETER_INPUT);

	Parameters.Add_Grid ("", "RANGE"    , _TL("Range"             ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid ("", "MEAN"     , _TL("Arithmetic Mean"   ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid ("", "VAR"      , _TL("Variance"          ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid ("", "STDDEV"   , _TL("Standard Deviation"), _TL(""), PARAMETER_OUTPUT);

	Parameters.Add_Grid_List("",
		"QUANTILES"	, _TL("Percentiles"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_String("",
		"QUANTVALS"	, _TL("Percentiles"),
		_TL("Separate the desired percentiles by semicolon"),
		"5; 25; 50; 75; 95"
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Statistics_Evaluate::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	pParameters->Set_Enabled("RANGE", (*pParameters)("MIN")->asGrid() && (*pParameters)("MAX")->asGrid());

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Statistics_Evaluate::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid	*pCount		= Parameters("COUNT" )->asGrid();
	CSG_Grid	*pMin		= Parameters("MIN"   )->asGrid();
	CSG_Grid	*pMax		= Parameters("MAX"   )->asGrid();
	CSG_Grid	*pSum		= Parameters("SUM"   )->asGrid();
	CSG_Grid	*pSum2		= Parameters("SUM2"  )->asGrid();

	CSG_Grid	*pRange		= pMin && pMax ? Parameters("RANGE" )->asGrid() : NULL;
	CSG_Grid	*pMean		= Parameters("MEAN"  )->asGrid();
	CSG_Grid	*pVar		= Parameters("VAR"   )->asGrid();
	CSG_Grid	*pStdDev	= Parameters("STDDEV")->asGrid();

	//-----------------------------------------------------
	double	hLag, hMin, hMax;

	CSG_Vector	Quantiles;

	CSG_Parameter_Grid_List	*pQuantiles	= Parameters("QUANTILES")->asGridList();

	CSG_Grids	*pHistogram	= Parameters("HISTOGRAM")->asGrids();

	if( pHistogram )
	{
		if( pHistogram->Get_NZ() < 2 )
		{
			Error_Set(_TL("histogram needs at least tow classes"));

			return( false );
		}

		hLag	= pHistogram->Get_ZRange() / (pHistogram->Get_NZ() - 1);
		hMin	= pHistogram->Get_ZMin() - 0.5 * hLag;
		hMax	= pHistogram->Get_ZMax() - 0.5 * hLag;
		hLag	= pHistogram->Get_NZ() / (hMax - hMin);

		//-------------------------------------------------
		pQuantiles->Del_Items();

		CSG_String_Tokenizer	Values(Parameters("QUANTVALS")->asString(), ";");

		while( Values.Has_More_Tokens() )
		{
			CSG_String	s(Values.Get_Next_Token()); s.Trim(true); s.Trim(false);
		
			double	v;

			if( s.asDouble(v) && v >= 0.0 && v <= 100.0 )
			{
				Quantiles.Add_Row(v / 100.0);

				CSG_Grid	*pQuantile	= SG_Create_Grid(*Get_System());

				pQuantile->Set_Name(CSG_String::Format("%s [%s]", _TL("Percentile"), s.c_str()));

				pQuantiles->Add_Item(pQuantile);
			}
		}
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#ifndef _DEBUG
		#pragma omp parallel for
		#endif
		for(int x=0; x<Get_NX(); x++)
		{
			if( pCount->is_NoData(x, y) )
			{
				if( pRange )
				{
					pRange->Set_NoData(x, y);
				}

				pMean  ->Set_NoData(x, y);
				pVar   ->Set_NoData(x, y);
				pStdDev->Set_NoData(x, y);

				for(int i=0; i<Quantiles.Get_N(); i++)
				{
					pQuantiles->Get_Grid(i)->Set_NoData(x, y);
				}
			}
			else
			{
				double	Count	= pCount->asDouble(x, y);
				double	Mean	= pSum ->asDouble(x, y) / Count;
				double	Var		= pSum2->asDouble(x, y) / Count - Mean*Mean;

				if( pRange )
				{
					pRange->Set_Value(x, y, pMax->asDouble(x, y) - pMin->asDouble(x, y));
				}

				pMean  ->Set_Value(x, y, Mean     );
				pVar   ->Set_Value(x, y, Var      );
				pStdDev->Set_Value(x, y, sqrt(Var));

				//-----------------------------------------
				CSG_Vector	Cumulative(pHistogram->Get_NZ());

				for(int z=0, Sum=0; z<pHistogram->Get_NZ(); z++)
				{
					Cumulative[z]	= (Sum += pHistogram->asInt(x, y, z));
				}

				for(int i=0; i<Quantiles.Get_N(); i++)
				{
					pQuantiles->Get_Grid(i)->Set_Value(x, y, Get_Quantile(Quantiles[i], Cumulative, hMin, hMax));
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
double CGrid_Statistics_Evaluate::Get_Quantile(double Quantile, const CSG_Vector &Cumulative, double Min, double Max)
{
	Quantile	*= Cumulative[Cumulative.Get_N() - 1];

	double	y = 0.0, dx	= (Max - Min) / Cumulative.Get_N();

	for(int i=0; i<Cumulative.Get_N(); i++)
	{
		if( Quantile < Cumulative[i] )
		{
			double	d	= Cumulative[i] > y ? (Quantile - y) / (Cumulative[i] - y) : 0.5;

			return( Min + dx * (i + d) );
		}
		else if( Quantile > Cumulative[i] )
		{
			y	= Cumulative[i];
		}
		else // if( Quantile == Cumulative[i] )
		{
			int	j	= i + 1; while( j < Cumulative.Get_N() && Quantile == Cumulative[j++] );
			
			return( Min + dx * (i + j - 1) / 2.0 );
		}
	}

	return( Max );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
