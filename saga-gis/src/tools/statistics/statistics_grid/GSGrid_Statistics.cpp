
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

	Set_Author		("O.Conrad (c) 2005");

	Set_Description	(_TW(
		"Calculates statistical properties (arithmetic mean, minimum, maximum, "
		"variance, standard deviation) for each cell position for the values of "
		"the selected grids.\n"
		"Optionally you can supply a list of grids with weights. If you want to "
		"use weights, the number of value and weight grids have to be the same "
		"Value and weight grids are associated by their order in the lists. "
		"Weight grids have not to share the grid system of the value grids. "
		"In case that no weight can be obtained from a weight grid for value, "
		"that value will be ignored. "
	));

	Parameters.Add_Grid_List("",
		"GRIDS"		, _TL("Values"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List("",
		"WEIGHTS"	, _TL("Weights"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL, false
	);

	Parameters.Add_Choice("WEIGHTS",
		"RESAMPLING", _TL("Resampling"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("Nearest Neighbour"),
			_TL("Bilinear Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 0
	);

	Parameters.Add_Grid("", "MEAN"    , _TL("Arithmetic Mean"             ), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid("", "MIN"     , _TL("Minimum"                     ), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid("", "MAX"     , _TL("Maximum"                     ), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid("", "RANGE"   , _TL("Range"                       ), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid("", "SUM"     , _TL("Sum"                         ), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid("", "SUM2"    , _TL("Sum2"                        ), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid("", "VAR"     , _TL("Variance"                    ), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid("", "STDDEV"  , _TL("Standard Deviation"          ), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid("", "STDDEVLO", _TL("Mean less Standard Deviation"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid("", "STDDEVHI", _TL("Mean plus Standard Deviation"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid("", "PCTL"    , _TL("Percentile"                  ), _TL(""), PARAMETER_OUTPUT_OPTIONAL);

	Parameters.Add_Double("PCTL",
		"PCTL_VAL"	, _TL("Percentile"),
		_TL(""),
		50., 0., true, 100., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGSGrid_Statistics::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("PCTL") )
	{
		pParameters->Set_Enabled("PCTL_VAL", pParameter->asPointer() != NULL);
	}

	if(	pParameter->Cmp_Identifier("WEIGHTS") )
	{
		pParameters->Set_Enabled("RESAMPLING", pParameter->asGridList()->Get_Grid_Count() > 0);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGSGrid_Statistics::On_Execute(void)
{
	CSG_Parameter_Grid_List	*pGrids	= Parameters("GRIDS")->asGridList();

	if( pGrids->Get_Grid_Count() <= 1 )
	{
		Error_Set(_TL("no grids in selection"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pWeights	= Parameters("WEIGHTS")->asGridList();

	if( pWeights->Get_Grid_Count() == 0 )
	{
		pWeights	= NULL;
	}
	else if( pWeights->Get_Grid_Count() != pGrids->Get_Grid_Count() )
	{
		Error_Set(_TL("number of weight grids have to be equal to the number of value grids"));

		return( false );
	}

	//-----------------------------------------------------
	TSG_Grid_Resampling	Resampling;

	switch( Parameters("RESAMPLING")->asInt() )
	{
	default: Resampling = GRID_RESAMPLING_NearestNeighbour; break;
	case  1: Resampling = GRID_RESAMPLING_Bilinear        ; break;
	case  2: Resampling = GRID_RESAMPLING_BicubicSpline   ; break;
	case  3: Resampling = GRID_RESAMPLING_BSpline         ; break;
	}

	//-----------------------------------------------------
	#define Get_Output(id) Parameters(id)->asGrid(); if( Parameters(id)->asGrid() ) bHasOutput = true;

	bool	bHasOutput	= false;

	CSG_Grid *pMean       = Get_Output("MEAN"    );
	CSG_Grid *pMin        = Get_Output("MIN"     );
	CSG_Grid *pMax        = Get_Output("MAX"     );
	CSG_Grid *pRange      = Get_Output("RANGE"   );
	CSG_Grid *pSum        = Get_Output("SUM"     );
	CSG_Grid *pSum2       = Get_Output("SUM2"    );
	CSG_Grid *pVar        = Get_Output("VAR"     );
	CSG_Grid *pStdDev     = Get_Output("STDDEV"  );
	CSG_Grid *pStdDevLo   = Get_Output("STDDEVLO");
	CSG_Grid *pStdDevHi   = Get_Output("STDDEVHI");
	CSG_Grid *pPercentile = Get_Output("PCTL"    );

	if( !bHasOutput )
	{
		Error_Set(_TL("no output parameter has been selected"));

		return( false );
	}

	double	Rank	= Parameters("PCTL_VAL")->asDouble();

	if( pPercentile )
	{
		pPercentile->Fmt_Name("%s [%.1f]", _TL("Percentile"), Rank);
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			CSG_Simple_Statistics s(pPercentile != NULL);

			for(int i=0; i<pGrids->Get_Grid_Count(); i++)
			{
				if( !pGrids->Get_Grid(i)->is_NoData(x, y) )
				{
					if( pWeights )
					{
						double	w = 0.;

						if( pWeights->Get_Grid(i)->Get_Value(Get_System().Get_Grid_to_World(x, y), w, Resampling) && w > 0. )
						{
							s.Add_Value(pGrids->Get_Grid(i)->asDouble(x, y), w);
						}
					}
					else
					{
						s.Add_Value(pGrids->Get_Grid(i)->asDouble(x, y));
					}
				}
			}

			//---------------------------------------------
			if( s.Get_Count() < 1 )
			{
				if( pMean       ) pMean      ->Set_NoData(x, y);
				if( pMin        ) pMin       ->Set_NoData(x, y);
				if( pMax        ) pMax       ->Set_NoData(x, y);
				if( pRange      ) pRange     ->Set_NoData(x, y);
				if( pSum        ) pSum       ->Set_NoData(x, y);
				if( pSum2       ) pSum2      ->Set_NoData(x, y);
				if( pVar        ) pVar       ->Set_NoData(x, y);
				if( pStdDev     ) pStdDev    ->Set_NoData(x, y);
				if( pStdDevLo   ) pStdDevLo  ->Set_NoData(x, y);
				if( pStdDevHi   ) pStdDevHi  ->Set_NoData(x, y);
				if( pPercentile ) pPercentile->Set_NoData(x, y);
			}
			else
			{
				if( pMean       ) pMean      ->Set_Value(x, y, s.Get_Mean          ());
				if( pMin        ) pMin       ->Set_Value(x, y, s.Get_Minimum       ());
				if( pMax        ) pMax       ->Set_Value(x, y, s.Get_Maximum       ());
				if( pRange      ) pRange     ->Set_Value(x, y, s.Get_Range         ());
				if( pSum        ) pSum       ->Set_Value(x, y, s.Get_Sum           ());
				if( pSum2       ) pSum2      ->Set_Value(x, y, s.Get_Sum_Of_Squares());
				if( pVar        ) pVar       ->Set_Value(x, y, s.Get_Variance      ());
				if( pStdDev     ) pStdDev    ->Set_Value(x, y, s.Get_StdDev        ());
				if( pStdDevLo   ) pStdDevLo  ->Set_Value(x, y, s.Get_Mean() - s.Get_StdDev());
				if( pStdDevHi   ) pStdDevHi  ->Set_Value(x, y, s.Get_Mean() + s.Get_StdDev());
				if( pPercentile ) pPercentile->Set_Value(x, y, s.Get_Percentile(Rank));
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
CGSGrid_Unique_Value_Statistics::CGSGrid_Unique_Value_Statistics(void)
{
	Set_Name		(_TL("Unique Value Statistics for Grids"));

	Set_Author		("O.Conrad (c) 2020");

	Set_Description	(_TW(
		"This tool analyzes for each cell position the uniquely appearing values "
		"of the input grids. Output is the number of unique values, the most "
		"frequent value (the majority), and the least frequent value (minority). "
	));

	Parameters.Add_Grid_List("",
		"GRIDS"		, _TL("Values"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("", "MAJORITY", _TL("Majority"               ), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid("", "MINORITY", _TL("Minority"               ), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid("", "NUNIQUES", _TL("Number of Unique Values"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGSGrid_Unique_Value_Statistics::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGSGrid_Unique_Value_Statistics::On_Execute(void)
{
	CSG_Parameter_Grid_List	*pGrids	= Parameters("GRIDS")->asGridList();

	if( pGrids->Get_Grid_Count() <= 1 )
	{
		Error_Set(_TL("no grids in selection"));

		return( false );
	}

	//-----------------------------------------------------
	#define Get_Output(id) Parameters(id)->asGrid(); if( Parameters(id)->asGrid() ) bHasOutput = true;

	bool	bHasOutput	= false;

	CSG_Grid *pMajority   = Get_Output("MAJORITY");
	CSG_Grid *pMinority   = Get_Output("MINORITY");
	CSG_Grid *pNUniques   = Get_Output("NUNIQUES");

	//-----------------------------------------------------
	if( !bHasOutput )
	{
		Error_Set(_TL("no output parameter has been selected"));

		return( false );
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			CSG_Unique_Number_Statistics s;

			for(int i=0; i<pGrids->Get_Grid_Count(); i++)
			{
				if( !pGrids->Get_Grid(i)->is_NoData(x, y) )
				{
					s	+= pGrids->Get_Grid(i)->asDouble(x, y);
				}
			}

			//---------------------------------------------
			if( s.Get_Count() < 1 )
			{
				if( pMajority ) pMajority->Set_NoData(x, y);
				if( pMinority ) pMinority->Set_NoData(x, y);
				if( pNUniques ) pNUniques->Set_NoData(x, y);
			}
			else
			{
				if( pMajority ) { double d; s.Get_Majority(d); pMajority->Set_Value(x, y, d); }
				if( pMinority ) { double d; s.Get_Minority(d); pMinority->Set_Value(x, y, d); }
				if( pNUniques ) pNUniques->Set_Value(x, y, s.Get_Count());
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
CGSGrid_Statistics_To_Table::CGSGrid_Statistics_To_Table(void)
{
	Set_Name		(_TL("Save Grid Statistics to Table"));

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Calculates statistical properties (arithmetic mean, minimum, maximum, "
		"variance, standard deviation) for each of the given grids and saves "
		"it to a table."
	));

	Parameters.Add_Grid_List("",
		"GRIDS"	, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table("",
		"STATS"	, _TL("Statistics for Grids"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Bool("", "DATA_CELLS"  , _TL("Number of Data Cells"        ), _TL(""), false);
	Parameters.Add_Bool("", "NODATA_CELLS", _TL("Number of No-Data Cells"     ), _TL(""), false);
	Parameters.Add_Bool("", "CELLSIZE"    , _TL("Cellsize"                    ), _TL(""), false);
	Parameters.Add_Bool("", "MEAN"        , _TL("Arithmetic Mean"             ), _TL(""),  true);
	Parameters.Add_Bool("", "MIN"         , _TL("Minimum"                     ), _TL(""),  true);
	Parameters.Add_Bool("", "MAX"         , _TL("Maximum"                     ), _TL(""),  true);
	Parameters.Add_Bool("", "RANGE"       , _TL("Range"                       ), _TL(""), false);
	Parameters.Add_Bool("", "SUM"         , _TL("Sum"                         ), _TL(""), false);
	Parameters.Add_Bool("", "SUM2"        , _TL("Sum of Squares"              ), _TL(""), false);
	Parameters.Add_Bool("", "VAR"         , _TL("Variance"                    ), _TL(""),  true);
	Parameters.Add_Bool("", "STDDEV"      , _TL("Standard Deviation"          ), _TL(""),  true);
	Parameters.Add_Bool("", "STDDEVLO"    , _TL("Mean less Standard Deviation"), _TL(""), false);
	Parameters.Add_Bool("", "STDDEVHI"    , _TL("Mean plus Standard Deviation"), _TL(""), false);

	Parameters.Add_String("",
		"PCTL_VAL"	, _TL("Percentiles"),
		_TL("Separate the desired percentiles by semicolon"),
		"5; 25; 50; 75; 95"
	);

	Parameters.Add_Bool("PCTL_VAL",
		"PCTL_HST"	, _TL("From Histogram"),
		_TL(""),
		true
	);

	Parameters.Add_Double("",
		"SAMPLES"	, _TL("Sample Size"),
		_TL("Minimum sample size [percent] used to calculate statistics. Ignored, if set to zero."),
		0., 0., true, 100., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGSGrid_Statistics_To_Table::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGSGrid_Statistics_To_Table::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("PCTL_VAL") )
	{
		pParameters->Set_Enabled("PCTL_HST", *pParameter->asString() != '\0');
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGSGrid_Statistics_To_Table::On_Execute(void)
{
	CSG_Parameter_Grid_List	*pGrids	= Parameters("GRIDS")->asGridList();

	if( pGrids->Get_Grid_Count() < 1 )
	{
		Error_Set(_TL("no grids in selection"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Table	*pTable	= Parameters("STATS")->asTable();

	pTable->Destroy();
	pTable->Set_Name(_TL("Statistics for Grids"));
	pTable->Add_Field(_TL("NAME"), SG_DATATYPE_String);

	if( Parameters("DATA_CELLS"  )->asBool() )	pTable->Add_Field(_TL("DATA_CELLS"  ), SG_DATATYPE_Int   );
	if( Parameters("NODATA_CELLS")->asBool() )	pTable->Add_Field(_TL("NODATA_CELLS"), SG_DATATYPE_Int   );
	if( Parameters("CELLSIZE"    )->asBool() )	pTable->Add_Field(_TL("CELLSIZE"    ), SG_DATATYPE_Double);
	if( Parameters("MEAN"        )->asBool() )	pTable->Add_Field(_TL("MEAN"        ), SG_DATATYPE_Double);
	if( Parameters("MIN"         )->asBool() )	pTable->Add_Field(_TL("MIN"         ), SG_DATATYPE_Double);
	if( Parameters("MAX"         )->asBool() )	pTable->Add_Field(_TL("MAX"         ), SG_DATATYPE_Double);
	if( Parameters("RANGE"       )->asBool() )	pTable->Add_Field(_TL("RANGE"       ), SG_DATATYPE_Double);
	if( Parameters("SUM"         )->asBool() )	pTable->Add_Field(_TL("SUM"         ), SG_DATATYPE_Double);
	if( Parameters("SUM2"        )->asBool() )	pTable->Add_Field(_TL("SUM2"        ), SG_DATATYPE_Double);
	if( Parameters("VAR"         )->asBool() )	pTable->Add_Field(_TL("VAR"         ), SG_DATATYPE_Double);
	if( Parameters("STDDEV"      )->asBool() )	pTable->Add_Field(_TL("STDDEV"      ), SG_DATATYPE_Double);
	if( Parameters("STDDEVLO"    )->asBool() )	pTable->Add_Field(_TL("STDDEVLO"    ), SG_DATATYPE_Double);
	if( Parameters("STDDEVHI"    )->asBool() )	pTable->Add_Field(_TL("STDDEVHI"    ), SG_DATATYPE_Double);

	//-----------------------------------------------------
	CSG_Table	Percentiles;

	Percentiles.Add_Field("FIELD", SG_DATATYPE_Int   );
	Percentiles.Add_Field("VALUE", SG_DATATYPE_Double);

	for(CSG_String_Tokenizer Values(Parameters("PCTL_VAL")->asString(), ";"); Values.Has_More_Tokens(); )
	{
		CSG_String s(Values.Get_Next_Token()); s.Trim_Both(); double v;

		if( s.asDouble(v) && v >= 0. && v <= 100. )
		{
			int n = Percentiles.Get_Count();

			Percentiles.Add_Record();

			Percentiles[n].Set_Value(0, pTable->Get_Field_Count());
			Percentiles[n].Set_Value(1, v);
			
			pTable->Add_Field(CSG_String::Format("%s%02d", _TL("PCTL"), (int)v), SG_DATATYPE_Double);
		}
	}

	//-----------------------------------------------------
	if( pTable->Get_Field_Count() <= 1 )
	{
		Error_Set(_TL("no parameter output has been selected"));

		return( false );
	}

	double Samples = Parameters("SAMPLES")->asDouble() / 100.;

	//-----------------------------------------------------
	for(int i=0; i<pGrids->Get_Grid_Count() && Process_Get_Okay(); i++)
	{
		CSG_Grid *pGrid = pGrids->Get_Grid(i); CSG_Table_Record *pRecord = pTable->Add_Record();

		CSG_Simple_Statistics s;

		sLong nSamples[2]; nSamples[0] = (sLong)(pGrid->Get_NCells() * Samples);

		if( nSamples[0] > (nSamples[1] = pGrid->Get_Max_Samples()) )
		{
			pGrid->Set_Max_Samples(nSamples[0]);
			s	= pGrid->Get_Statistics();
			pGrid->Set_Max_Samples(nSamples[1]);	// restore old sample size
		}
		else
		{
			s	= pGrid->Get_Statistics();
		}

		pRecord->Set_Value("NAME"        , pGrid->Get_Name             ());
		pRecord->Set_Value("DATA_CELLS"  , (int)(pGrid->Get_NCells() - pGrid->Get_NoData_Count()));
		pRecord->Set_Value("NODATA_CELLS", (int)(                      pGrid->Get_NoData_Count()));
		pRecord->Set_Value("CELLSIZE"    , pGrid->Get_Cellsize         ());
		pRecord->Set_Value("MEAN"        , s.Get_Mean                  ());
		pRecord->Set_Value("MIN"         , s.Get_Minimum               ());
		pRecord->Set_Value("MAX"         , s.Get_Maximum               ());
		pRecord->Set_Value("RANGE"       , s.Get_Range                 ());
		pRecord->Set_Value("SUM"         , s.Get_Sum                   ());
		pRecord->Set_Value("SUM2"        , s.Get_Sum_Of_Squares        ());
		pRecord->Set_Value("VAR"         , s.Get_Variance              ());
		pRecord->Set_Value("STDDEV"      , s.Get_StdDev                ());
		pRecord->Set_Value("STDDEVLO"    , s.Get_Mean() - s.Get_StdDev ());
		pRecord->Set_Value("STDDEVHI"    , s.Get_Mean() + s.Get_StdDev ());

		for(int j=0; j<Percentiles.Get_Count(); j++)
		{
			pRecord->Set_Value(Percentiles[j].asInt(0),
				pGrid->Get_Percentile(Percentiles[j].asDouble(1), Parameters("PCTL_HST")->asBool())
			);
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
CGSGrid_Histogram::CGSGrid_Histogram(void)
{
	Set_Name		(_TL("Grid Histogram"));

	Set_Author		("O.Conrad (c) 2022");

	Set_Description	(_TW(
		"This tool creates a histogram for the supplied grid using the specified classification. "
	));

	Parameters.Add_Grid("",
		"GRID"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table("",
		"HISTOGRAM"	, _TL("Histogram"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice("",
		"CLASSIFY"	, _TL("Classification"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("value range and number of classes"),
			_TL("lookup table")
		), 0
	);

	Parameters.Add_Int("CLASSIFY",
		"BINS"		, _TL("Number of Classes"),
		_TL(""),
		64, 1, true
	);

	Parameters.Add_Range("CLASSIFY",
		"RANGE"		, _TL("Value Range"),
		_TL(""),
		0., 1.
	);

	Parameters.Add_FixedTable("CLASSIFY",
		"LUT"		, _TL("Lookup Table"),
		_TL("")
	);

	Parameters.Add_Bool("",
		"UNCLASSED"	, _TL("Report Unclassified Cells"),
		_TL(""),
		false
	);

	Parameters.Add_Bool("",
		"PARALLEL"	, _TL("Parallelized"),
		_TL(""),
		true
	);

	Parameters.Add_Double("",
		"MAXSAMPLES", _TL("Maximum Samples"),
		_TL("Maximum number of samples [percent]."),
		100., 0., true, 100., true
	);

	//-----------------------------------------------------
	CSG_Table &LUT = *Parameters("LUT")->asTable();
	LUT.Add_Field("Minimum", SG_DATATYPE_Double);
	LUT.Add_Field("Maximum", SG_DATATYPE_Double);
	LUT.Set_Count(1); LUT[0].Set_Value(0, 0.); LUT[0].Set_Value(1, 1.);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGSGrid_Histogram::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("GRID") && pParameter->asGrid() )
	{
		(*pParameters)("RANGE")->asRange()->Set_Range(
			pParameter->asGrid()->Get_Min(),
			pParameter->asGrid()->Get_Max()
		);

		if( has_GUI() )
		{
			pParameters->Set_Parameter("MAXSAMPLES", 1e09 / pParameter->asGrid()->Get_NCells()); // 10 mio. samples
		}
	}

	return( CSG_Tool_Grid::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGSGrid_Histogram::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("CLASSIFY") )
	{
		pParameters->Set_Enabled("RANGE", pParameter->asInt() == 0);
		pParameters->Set_Enabled("BINS" , pParameter->asInt() == 0);
		pParameters->Set_Enabled("LUT"  , pParameter->asInt() == 1);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGSGrid_Histogram::On_Execute(void)
{
	CSG_Grid *pGrid = Parameters("GRID")->asGrid();

	CSG_Table &Histogram = Get_Table();

	bool bUnclassed = Parameters("UNCLASSED")->asBool();

	//-----------------------------------------------------
	double Samples = 0.01 * Parameters("MAXSAMPLES")->asDouble();

	if( Samples <= 1. / Get_NX() )
	{
		Samples  = 1. / Get_NX(); // at least one sample per line!
	}
	else if( Samples > 1. )
	{
		Samples  = 1.;
	}

	int Step = (int)(Get_NX() / (Get_NX() * Samples));

	//-----------------------------------------------------
	if( Parameters("PARALLEL")->asBool() == false )
	{
		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(int x=0; x<Get_NX(); x+=Step)
			{
				Add_Value(Histogram, pGrid->asDouble(x, y), bUnclassed);
			}
		}
	}
	else
	{
		int nThreads = SG_OMP_Get_Max_Num_Threads();

		CSG_Table *Histograms = new CSG_Table[nThreads];

		for(int iThread=0; iThread<nThreads; iThread++)
		{
			Histograms[iThread].Create(Histogram);
		}

		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			#pragma omp parallel for
			for(int x=0; x<Get_NX(); x+=Step)
			{
				int iThread = SG_OMP_Get_Thread_Num();

				Add_Value(Histograms[iThread], pGrid->asDouble(x, y), bUnclassed);
			}
		}

		for(int iThread=0; iThread<nThreads; iThread++)
		{
			for(int i=0; i<Histogram.Get_Count(); i++)
			{
				Histogram[i].Add_Value(FIELD_COUNT, Histograms[iThread][i].asInt(FIELD_COUNT));
			}
		}

		delete[](Histograms);
	}

	//-----------------------------------------------------
	for(int i=0; i<Histogram.Get_Count(); i++)
	{
		if( Samples < 1. )
		{
			Histogram[i].Mul_Value(FIELD_COUNT, 1. / Samples);
		}

		Histogram[i].Set_Value(FIELD_AREA, Histogram[i].asInt(FIELD_COUNT) * Get_Cellarea());

		if( i < 1 )
		{
			Histogram[i].Set_Value(FIELD_CUMUL, Histogram[i].asInt(FIELD_COUNT));
		}
		else if( !bUnclassed || i < Histogram.Get_Count() - 1 )
		{
			Histogram[i].Set_Value(FIELD_CUMUL, Histogram[i].asInt(FIELD_COUNT) + Histogram[i - 1].asInt(FIELD_CUMUL));
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGSGrid_Histogram::Add_Value(CSG_Table &Histogram, double Value, bool bUnclassed)
{
	int n = bUnclassed ? Histogram.Get_Count() - 1 : Histogram.Get_Count();

	for(int i=0; i<n; i++)
	{
		if( Histogram[i].asDouble(FIELD_MIN) <= Value && Value <= Histogram[i].asDouble(FIELD_MAX) )
		{
			Histogram[i].Add_Value(FIELD_COUNT, 1);

			return( true );
		}
	}

	if( bUnclassed )
	{
		Histogram[n].Add_Value(FIELD_COUNT, 1);
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Table & CGSGrid_Histogram::Get_Table(void)
{
	CSG_Table &Histogram = *Parameters("HISTOGRAM")->asTable();

	Histogram.Destroy();
	Histogram.Fmt_Name("%s [%s]", _TL("Histogram"), Parameters("GRID")->asGrid()->Get_Name());
	Histogram.Add_Field("ID"        , SG_DATATYPE_Int   ); // FIELD_ID
	Histogram.Add_Field("Class"     , SG_DATATYPE_String); // FIELD_CLASS
	Histogram.Add_Field("Minimum"   , SG_DATATYPE_Double); // FIELD_MIN
	Histogram.Add_Field("Maximum"   , SG_DATATYPE_Double); // FIELD_MAX
	Histogram.Add_Field("Count"     , SG_DATATYPE_Long  ); // FIELD_COUNT
	Histogram.Add_Field("Cumulative", SG_DATATYPE_Long  ); // FIELD_CUMUL
	Histogram.Add_Field("Area"      , SG_DATATYPE_Double); // FIELD_AREA

	switch( Parameters("CLASSIFY")->asInt() )
	{
	default: {
		int    n =  Parameters("BINS")->asInt();
		double a =  Parameters("RANGE.MIN")->asDouble();
		double b = (Parameters("RANGE.MAX")->asDouble() - a) / (double)n;

		for(int i=0; i<n; i++)
		{
			double min = a + b * i, max = a + b * (i + 1);

			CSG_Table_Record &Class = *Histogram.Add_Record();

			Class.Set_Value(FIELD_ID   , i + 1);
			Class.Set_Value(FIELD_CLASS, SG_Get_String(min, -6) + " - " + SG_Get_String(max, -6));
			Class.Set_Value(FIELD_MIN  , a + (i    ) * b);
			Class.Set_Value(FIELD_MAX  , a + (i + 1) * b);
		}
		break; }

	case  1: {
		CSG_Table &LUT = *Parameters("LUT")->asTable();

		for(int i=0; i<LUT.Get_Count(); i++)
		{
			CSG_Table_Record &Class = *Histogram.Add_Record();

			Class.Set_Value(FIELD_ID   , i + 1);
			Class.Set_Value(FIELD_CLASS, CSG_String::Format("%s - %s", LUT[i].asString(0), LUT[i].asString(1)));
			Class.Set_Value(FIELD_MIN  , LUT[i].asDouble(0));
			Class.Set_Value(FIELD_MAX  , LUT[i].asDouble(1));
		}
		break; }
	}

	if( Parameters("UNCLASSED")->asBool() )
	{
		CSG_Table_Record &Class = *Histogram.Add_Record();

		Class.Set_Value (FIELD_ID   , -1);
		Class.Set_Value (FIELD_CLASS, "Unclassified");
		Class.Set_NoData(FIELD_MIN  );
		Class.Set_NoData(FIELD_MAX  );
		Class.Set_NoData(FIELD_CUMUL);
	}

	return( Histogram );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
