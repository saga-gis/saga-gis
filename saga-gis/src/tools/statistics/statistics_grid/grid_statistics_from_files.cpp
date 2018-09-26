
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
//             grid_statistics_from_files.cpp            //
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
#include "grid_statistics_from_files.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Statistics_from_Files::CGrid_Statistics_from_Files(void)
{
	Set_Name		(_TL("Statistics for Grids from Files"));

	Set_Author		("O.Conrad (c) 2018");

	Set_Description	(_TW(
		"Calculates statistical properties (arithmetic mean, minimum, maximum, "
		"variance, standard deviation) for each cell position for the values of "
		"the selected grids. This tool works file based to allow the processing "
		"of a large number of grids. "
	));

	Parameters.Add_Grid_System("", "GRID_SYSTEM", _TL("Grid System"), _TL(""));

	Parameters.Add_Grid("GRID_SYSTEM", "COUNT" , _TL("Number of Values"  ), _TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Word);
	Parameters.Add_Grid("GRID_SYSTEM", "MIN"   , _TL("Minimum"           ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid("GRID_SYSTEM", "MAX"   , _TL("Maximum"           ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid("GRID_SYSTEM", "RANGE" , _TL("Range"             ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid("GRID_SYSTEM", "SUM"   , _TL("Sum"               ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid("GRID_SYSTEM", "SUM2"  , _TL("Sum of Squares"    ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid("GRID_SYSTEM", "MEAN"  , _TL("Arithmetic Mean"   ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid("GRID_SYSTEM", "VAR"   , _TL("Variance"          ), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grid("GRID_SYSTEM", "STDDEV", _TL("Standard Deviation"), _TL(""), PARAMETER_OUTPUT);

	Parameters.Add_Grids("GRID_SYSTEM",
		"HISTOGRAM"	, _TL("Histogram"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Word
	);

	Parameters.Add_FilePath("",
		"FILES"		, _TL("Grid Files"),
		_TL(""),
		CSG_String::Format("%s|*.sg-grd;*.sg-grd-z;*.sgrd|%s|*.*", _TL("SAGA Grids"), _TL("All Files")),
		NULL, false, false, true
	);

	Parameters.Add_Node("", "HISTOGRAM_OPTIONS", _TL("Histogram"), _TL(""));

	Parameters.Add_Int("HISTOGRAM_OPTIONS",
		"HCLASSES"	, _TL("Number of Classes"),
		_TL(""),
		20, 2, true
	);

	Parameters.Add_Choice("HISTOGRAM_OPTIONS",
		"HRANGE"	, _TL("Range"),
		_TL("Minimum and maximum values used to define the histogram classes."),
		CSG_String::Format("%s|%s",
			_TL("overall"),
			_TL("cell")
		), 1
	);

	Parameters.Add_Bool("HISTOGRAM_OPTIONS",
		"CUMULATIVE", _TL("Cumulative"),
		_TL(""),
		false
	);

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
int CGrid_Statistics_from_Files::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("GRID_SYSTEM") )
	{
		pParameters->Set_Enabled("COUNT"    , pParameter->asGrid_System()->is_Valid());
		pParameters->Set_Enabled("MIN"      , pParameter->asGrid_System()->is_Valid());
		pParameters->Set_Enabled("MAX"      , pParameter->asGrid_System()->is_Valid());
		pParameters->Set_Enabled("RANGE"    , pParameter->asGrid_System()->is_Valid());
		pParameters->Set_Enabled("SUM"      , pParameter->asGrid_System()->is_Valid());
		pParameters->Set_Enabled("SUM2"     , pParameter->asGrid_System()->is_Valid());
		pParameters->Set_Enabled("MEAN"     , pParameter->asGrid_System()->is_Valid());
		pParameters->Set_Enabled("VAR"      , pParameter->asGrid_System()->is_Valid());
		pParameters->Set_Enabled("STDDEV"   , pParameter->asGrid_System()->is_Valid());
		pParameters->Set_Enabled("HISTOGRAM", pParameter->asGrid_System()->is_Valid());
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Statistics_from_Files::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Strings	Files;

	if( !Parameters("FILES")->asFilePath()->Get_FilePaths(Files) )
	{
		Error_Set(_TL("no grids in selection"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid	*pCount, *pMin, *pMax, *pRange, *pSum, *pSum2, *pMean, *pVar, *pStdDev;

	CSG_Grid_System	System(*Parameters("GRID_SYSTEM")->asGrid_System());

	if( System.is_Valid() )
	{
		pCount	= Parameters("COUNT" )->asGrid();
		pMin	= Parameters("MIN"   )->asGrid();
		pMax	= Parameters("MAX"   )->asGrid();
		pRange	= Parameters("RANGE" )->asGrid();
		pSum	= Parameters("SUM"   )->asGrid();
		pSum2	= Parameters("SUM2"  )->asGrid();
		pMean	= Parameters("MEAN"  )->asGrid();
		pVar	= Parameters("VAR"   )->asGrid();
		pStdDev	= Parameters("STDDEV")->asGrid();

		pCount->Assign(0.0);
		pCount->Set_NoData_Value(0.0);
	}

	//-----------------------------------------------------
	for(int	iFile=0; iFile<Files.Get_Count() && Set_Progress(iFile, Files.Get_Count()); iFile++)
	{
		SG_UI_Process_Set_Text(CSG_String::Format("%s: %s", _TL("building basic statistics"), SG_File_Get_Name(Files[iFile], false).c_str()));

		SG_UI_ProgressAndMsg_Lock(true);
		CSG_Grid	Grid(Files[iFile]);
		SG_UI_ProgressAndMsg_Lock(false);
		
		if( !Grid.is_Valid() )
		{
			Message_Fmt("\n%s: %s", _TL("failed to load"), Files[iFile].c_str());

			Files[iFile].Clear();

			continue;
		}

		if( System.is_Valid() && !System.is_Equal(Grid.Get_System()) )
		{
			Message_Fmt("\n%s: %s", _TL("incompatible grid system"), Files[iFile].c_str());

			Files[iFile].Clear();

			continue;
		}

		if( !System.is_Valid() )
		{
			System.Assign(Grid.Get_System());

			Parameters("COUNT" )->Set_Value(pCount  = SG_Create_Grid(System, SG_DATATYPE_Word)); pCount->Set_Name(_TL("Number of Values")); pCount->Set_NoData_Value(0.0);
			Parameters("MIN"   )->Set_Value(pMin    = SG_Create_Grid(System)); pMin   ->Set_Name(_TL("Minimum"           ));
			Parameters("MAX"   )->Set_Value(pMax    = SG_Create_Grid(System)); pMax   ->Set_Name(_TL("Maximum"           ));
			Parameters("RANGE" )->Set_Value(pRange  = SG_Create_Grid(System)); pRange ->Set_Name(_TL("Range"             ));
			Parameters("SUM"   )->Set_Value(pSum    = SG_Create_Grid(System)); pSum   ->Set_Name(_TL("Sum"               ));
			Parameters("SUM2"  )->Set_Value(pSum2   = SG_Create_Grid(System)); pSum2  ->Set_Name(_TL("Sum of Squares"    ));
			Parameters("MEAN"  )->Set_Value(pMean   = SG_Create_Grid(System)); pMean  ->Set_Name(_TL("Arithmetic Mean"   ));
			Parameters("VAR"   )->Set_Value(pVar    = SG_Create_Grid(System)); pVar   ->Set_Name(_TL("Variance"          ));
			Parameters("STDDEV")->Set_Value(pStdDev = SG_Create_Grid(System)); pStdDev->Set_Name(_TL("Standard Deviation"));

			pCount->Get_Projection().Create(Grid.Get_Projection());
		}

		//-------------------------------------------------
		#ifndef _DEBUG
		#pragma omp parallel for
		#endif
		for(sLong i=0; i<System.Get_NCells(); i++)
		{
			if( !Grid.is_NoData(i) )
			{
				double	Value	= Grid.asDouble(i);

				if( pCount->asInt(i) < 1 )
				{
					pCount->Set_Value(i, 1.);
					pSum  ->Set_Value(i, Value);
					pSum2 ->Set_Value(i, Value*Value);

					pMin  ->Set_Value(i, Value);
					pMax  ->Set_Value(i, Value);
				}
				else
				{
					pCount->Add_Value(i, 1.);
					pSum  ->Add_Value(i, Value);
					pSum2 ->Add_Value(i, Value*Value);

					if( Value < pMin->asDouble(i) ) { pMin->Set_Value(i, Value); } else
					if( Value > pMax->asDouble(i) ) { pMax->Set_Value(i, Value); }
				}
			}
		}
	}

	if( !System.is_Valid() || pCount->Get_Max() < 1 )
	{
		Error_Set(_TL("failed to build basic statistics"));

		return( false );
	}

	//-----------------------------------------------------
	#ifndef _DEBUG
	#pragma omp parallel for
	#endif
	for(sLong i=0; i<System.Get_NCells(); i++)
	{
		int	Count	= pCount->asInt(i);

		if( Count < 1 )
		{
			pMin   ->Set_NoData(i);
			pMax   ->Set_NoData(i);
			pSum   ->Set_NoData(i);
			pSum2  ->Set_NoData(i);

			pRange ->Set_NoData(i);
			pMean  ->Set_NoData(i);
			pVar   ->Set_NoData(i);
			pStdDev->Set_NoData(i);
		}
		else
		{
			double	Mean	= pSum ->asDouble(i) / Count;
			double	Var		= pSum2->asDouble(i) / Count - Mean*Mean;

			pRange ->Set_Value(i, pMax->asDouble(i) - pMin->asDouble(i));
			pMean  ->Set_Value(i, Mean     );
			pVar   ->Set_Value(i, Var      );
			pStdDev->Set_Value(i, sqrt(Var));
		}
	}

	//-----------------------------------------------------
	Get_Histogram(Files, System);

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
CSG_Grids * CGrid_Statistics_from_Files::Get_Histogram(const CSG_Strings &Files, const CSG_Grid_System &System)
{
	CSG_Grid	*pMin	= Parameters("MIN")->asGrid();	double	abs_Min	= pMin->Get_Min();
	CSG_Grid	*pMax	= Parameters("MAX")->asGrid();	double	abs_Max	= pMax->Get_Max();

	if( abs_Min >= abs_Max )
	{
		Error_Set(_TL("The absolute minimum and maximum values are equal."));

		return( NULL );
	}

	//-----------------------------------------------------
	bool	bAbsolute	= Parameters("HRANGE")->asInt() == 0;
	bool	bCumulative	= Parameters("CUMULATIVE")->asBool();

	//-----------------------------------------------------
	int	n	= Parameters("HCLASSES")->asInt();

	CSG_Grids	*pHistogram	= Parameters("HISTOGRAM")->asGrids();

	if( !pHistogram )
	{
		Parameters("HISTOGRAM")->Set_Value(pHistogram = SG_Create_Grids(System, n, 0.0, SG_DATATYPE_Word));
	}
	else
	{
		pHistogram->Create(System, n, 0.0, SG_DATATYPE_Word);	// pHistogram->Assign(0.0);
	}

	pHistogram->Set_Name(_TL("Histogram"));

	for(int z=0; z<n; z++)
	{
		pHistogram->Set_Z(z, bCumulative ? z : abs_Min + (z + 0.5) * n / (abs_Max - abs_Min));
	}

	//-----------------------------------------------------
	for(int iFile=0; iFile<Files.Get_Count() && Set_Progress(iFile, Files.Get_Count()); iFile++)
	{
		SG_UI_Process_Set_Text(CSG_String::Format("%s: %s", _TL("building histogram"), SG_File_Get_Name(Files[iFile], false).c_str()));

		SG_UI_ProgressAndMsg_Lock(true);
		CSG_Grid	Grid(Files[iFile]);
		SG_UI_ProgressAndMsg_Lock(false);
		
		if( !System.is_Equal(Grid.Get_System()) )
		{
			continue;
		}

		//-------------------------------------------------
		#ifndef _DEBUG
		#pragma omp parallel for
		#endif
		for(int y=0; y<System.Get_NY(); y++) for(int x=0; x<System.Get_NX(); x++)
		{
			if( !Grid.is_NoData(x, y) )
			{
				int	z;
				
				if( bAbsolute )
				{
					z	= (int)(0.5 + (Grid.asDouble(x, y) - abs_Min) * n / (abs_Max - abs_Min));
				}
				else
				{
					double	Min	= pMin->asDouble(x, y);
					double	Max	= pMax->asDouble(x, y);

					z	= Min < Max ? (int)(0.5 + (Grid.asDouble(x, y) - Min) * n / (Max - Min)) : -1;
				}

				if( z >= 0 && z < pHistogram->Get_NZ() )
				{
					pHistogram->Add_Value(x, y, z, 1.);
				}
			}
		}
	}

	//-----------------------------------------------------
	CSG_Vector	Quantiles;

	CSG_Parameter_Grid_List	*pQuantiles	= Parameters("QUANTILES")->asGridList();

	pQuantiles->Del_Items();

	CSG_String_Tokenizer	Values(Parameters("QUANTVALS")->asString(), ";");

	while( Values.Has_More_Tokens() )
	{
		CSG_String	s(Values.Get_Next_Token()); s.Trim(true); s.Trim(false);
		
		double	v;

		if( s.asDouble(v) && v >= 0.0 && v <= 100.0 )
		{
			Quantiles.Add_Row(v / 100.0);

			CSG_Grid	*pQuantile	= SG_Create_Grid(System);

			pQuantile->Set_Name("%s [%s]", _TL("Percentile"), s.c_str());

			pQuantiles->Add_Item(pQuantile);
		}
	}

	//-----------------------------------------------------
	#ifndef _DEBUG
	#pragma omp parallel for
	#endif
	for(int y=0; y<System.Get_NY(); y++) for(int x=0; x<System.Get_NX(); x++)
	{
		if( pMin->is_NoData(x, y) || pMax->is_NoData(x, y) )
		{
			for(int i=0; i<Quantiles.Get_N(); i++)
			{
				pQuantiles->Get_Grid(i)->Set_NoData(x, y);
			}

			for(int z=0; z<pHistogram->Get_NZ(); z++)
			{
				pHistogram->Set_NoData(x, y, z);
			}
		}
		else
		{
			if( Quantiles.Get_N() > 0 )
			{
				double	Min	= bAbsolute ? pMin->Get_Min() : pMin->asDouble(x, y);
				double	Max	= bAbsolute ? pMax->Get_Max() : pMax->asDouble(x, y);

				if( Min < Max )
				{
					CSG_Vector	Cumulative(pHistogram->Get_NZ());

					for(int z=0, Sum=0; z<pHistogram->Get_NZ(); z++)
					{
						Cumulative[z]	= (Sum += pHistogram->asInt(x, y, z));
					}

					for(int i=0; i<Quantiles.Get_N(); i++)
					{
						pQuantiles->Get_Grid(i)->Set_Value(x, y, Get_Quantile(Quantiles[i], Cumulative, Min, Max));
					}
				}
				else for(int i=0; i<Quantiles.Get_N(); i++)
				{
					pQuantiles->Get_Grid(i)->Set_Value(x, y, Min);
				}
			}

			if( bCumulative )
			{
				for(int z=0, Sum=0; z<pHistogram->Get_NZ(); z++)
				{
					pHistogram->Set_Value(x, y, z, Sum += pHistogram->asInt(x, y, z));
				}
			}
		}
	}

	//-----------------------------------------------------
	return( pHistogram );
}

//---------------------------------------------------------
double CGrid_Statistics_from_Files::Get_Quantile(double Quantile, const CSG_Vector &Cumulative, double Min, double Max)
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
