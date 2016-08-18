/**********************************************************
 * Version $Id: GSGrid_Statistics.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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

	Set_Description	(_TW(
		"Calculates statistical properties (arithmetic mean, minimum, maximum, "
		"variance, standard deviation) for each cell position for the values of "
		"the selected grids."
	));


	Parameters.Add_Grid_List(
		NULL, "GRIDS"	, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(NULL, "MEAN"	, _TL("Arithmetic Mean")             , _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid(NULL, "MIN"		, _TL("Minimum")                     , _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid(NULL, "MAX"		, _TL("Maximum")                     , _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid(NULL, "RANGE"	, _TL("Range")                       , _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid(NULL, "SUM"		, _TL("Sum")                         , _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid(NULL, "VAR"		, _TL("Variance")                    , _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid(NULL, "STDDEV"	, _TL("Standard Deviation")          , _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid(NULL, "STDDEVLO", _TL("Mean less Standard Deviation"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid(NULL, "STDDEVHI", _TL("Mean plus Standard Deviation"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid(NULL, "PCTL"	, _TL("Percentile")                  , _TL(""), PARAMETER_OUTPUT_OPTIONAL);

	Parameters.Add_Value(
		NULL, "PCTL_VAL", _TL("Percentile"),
		_TL(""),
		PARAMETER_TYPE_Double, 50.0, 0.0, true, 100.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGSGrid_Statistics::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("PCTL")) )
	{
		pParameters->Get_Parameter("PCTL_VAL")->Set_Enabled(pParameter->asGrid() != NULL);
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGSGrid_Statistics::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pGrids	= Parameters("GRIDS")->asGridList();

	if( pGrids->Get_Count() <= 1 )
	{
		Error_Set(_TL("no grids in selection"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid	*pMean			= Parameters("MEAN"    )->asGrid();
	CSG_Grid	*pMin			= Parameters("MIN"     )->asGrid();
	CSG_Grid	*pMax			= Parameters("MAX"     )->asGrid();
	CSG_Grid	*pRange			= Parameters("RANGE"   )->asGrid();
	CSG_Grid	*pSum			= Parameters("SUM"     )->asGrid();
	CSG_Grid	*pVar			= Parameters("VAR"     )->asGrid();
	CSG_Grid	*pStdDev		= Parameters("STDDEV"  )->asGrid();
	CSG_Grid	*pStdDevLo		= Parameters("STDDEVLO")->asGrid();
	CSG_Grid	*pStdDevHi		= Parameters("STDDEVHI")->asGrid();
	CSG_Grid	*pPercentile	= Parameters("PCTL"    )->asGrid();

	if( !pMean && !pMin && !pMax && !pRange && !pSum && !pVar && !pStdDev && !pStdDevLo && !pStdDevHi && !pPercentile )
	{
		Error_Set(_TL("no parameter output specified"));

		return( false );
	}

	double	dRank	= Parameters("PCTL_VAL")->asDouble() / 100.0;

	//-----------------------------------------------------
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
				if( pRange      )	pRange		->Set_NoData(x, y);
				if( pSum        )	pSum		->Set_NoData(x, y);
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
				if( pRange      )	pRange		->Set_Value(x, y, s.Get_Range());
				if( pSum        )	pSum		->Set_Value(x, y, s.Get_Sum());
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

	Set_Author		(SG_T("O.Conrad (c) 2013"));

	Set_Description	(_TW(
		"Calculates statistical properties (arithmetic mean, minimum, maximum, "
		"variance, standard deviation) for each of the given grids and saves "
		"it to a table."
	));


	Parameters.Add_Grid_List(
		NULL, "GRIDS"	, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table(
		NULL, "STATS"	, _TL("Statistics for Grids"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(NULL, "DATA_CELLS"  , _TL("Number of Data Cells")        , _TL(""), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(NULL, "NODATA_CELLS", _TL("Number of No-Data Cells")     , _TL(""), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(NULL, "CELLSIZE"    , _TL("Cellsize")                    , _TL(""), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(NULL, "MEAN"        , _TL("Arithmetic Mean")             , _TL(""), PARAMETER_TYPE_Bool, true);
	Parameters.Add_Value(NULL, "MIN"         , _TL("Minimum")                     , _TL(""), PARAMETER_TYPE_Bool, true);
	Parameters.Add_Value(NULL, "MAX"         , _TL("Maximum")                     , _TL(""), PARAMETER_TYPE_Bool, true);
	Parameters.Add_Value(NULL, "RANGE"       , _TL("Range")                       , _TL(""), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(NULL, "VAR"         , _TL("Variance")                    , _TL(""), PARAMETER_TYPE_Bool, true);
	Parameters.Add_Value(NULL, "STDDEV"      , _TL("Standard Deviation")          , _TL(""), PARAMETER_TYPE_Bool, true);
	Parameters.Add_Value(NULL, "STDDEVLO"    , _TL("Mean less Standard Deviation"), _TL(""), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(NULL, "STDDEVHI"    , _TL("Mean plus Standard Deviation"), _TL(""), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(NULL, "PCTL"        , _TL("Percentile")                  , _TL(""), PARAMETER_TYPE_Bool, false);

	Parameters.Add_Value(
		NULL, "PCTL_VAL", _TL("Percentile"),
		_TL(""),
		PARAMETER_TYPE_Double, 50.0, 0.0, true, 100.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGSGrid_Statistics_To_Table::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("PCTL")) )
	{
		pParameters->Get_Parameter("PCTL_VAL")->Set_Enabled(pParameter->asBool());
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGSGrid_Statistics_To_Table::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pGrids	= Parameters("GRIDS")->asGridList();

	if( pGrids->Get_Count() < 1 )
	{
		Error_Set(_TL("no grids in selection"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Table	*pTable	= Parameters("STATS")->asTable();

	pTable->Destroy();
	pTable->Set_Name(_TL("Statistics for Grids"));
	pTable->Add_Field(_TL("NAME"), SG_DATATYPE_String);

	if( Parameters("DATA_CELLS"  )->asBool() )	pTable->Add_Field(_TL("DATA_CELLS"  ), SG_DATATYPE_Int);
	if( Parameters("NODATA_CELLS")->asBool() )	pTable->Add_Field(_TL("NODATA_CELLS"), SG_DATATYPE_Int);
	if( Parameters("CELLSIZE"    )->asBool() )	pTable->Add_Field(_TL("CELLSIZE"    ), SG_DATATYPE_Double);
	if( Parameters("MEAN"        )->asBool() )	pTable->Add_Field(_TL("MEAN"        ), SG_DATATYPE_Double);
	if( Parameters("MIN"         )->asBool() )	pTable->Add_Field(_TL("MIN"         ), SG_DATATYPE_Double);
	if( Parameters("MAX"         )->asBool() )	pTable->Add_Field(_TL("MAX"         ), SG_DATATYPE_Double);
	if( Parameters("RANGE"       )->asBool() )	pTable->Add_Field(_TL("RANGE"       ), SG_DATATYPE_Double);
	if( Parameters("VAR"         )->asBool() )	pTable->Add_Field(_TL("VAR"         ), SG_DATATYPE_Double);
	if( Parameters("STDDEV"      )->asBool() )	pTable->Add_Field(_TL("STDDEV"      ), SG_DATATYPE_Double);
	if( Parameters("STDDEVLO"    )->asBool() )	pTable->Add_Field(_TL("STDDEVLO"    ), SG_DATATYPE_Double);
	if( Parameters("STDDEVHI"    )->asBool() )	pTable->Add_Field(_TL("STDDEVHI"    ), SG_DATATYPE_Double);
	if( Parameters("PCTL"        )->asBool() )	pTable->Add_Field(_TL("PCTL"        ), SG_DATATYPE_Double);

	if( pTable->Get_Field_Count() <= 1 )
	{
		Error_Set(_TL("no parameter output specified"));

		return( false );
	}

	double	dRank	= Parameters("PCTL")->asBool() ? Parameters("PCTL_VAL")->asDouble() : -1.0;

	//-----------------------------------------------------
	for(int i=0; i<pGrids->Get_Count() && Process_Get_Okay(); i++)
	{
		CSG_Grid			*pGrid		= pGrids->asGrid(i);
		CSG_Table_Record	*pRecord	= pTable->Add_Record();

		pRecord->Set_Value("NAME"        , pGrid->Get_Name());
		pRecord->Set_Value("DATA_CELLS"  , pGrid->Get_NCells() - pGrid->Get_NoData_Count());
		pRecord->Set_Value("NODATA_CELLS", pGrid->Get_NoData_Count());
		pRecord->Set_Value("CELLSIZE"    , pGrid->Get_Cellsize());
		pRecord->Set_Value("MEAN"        , pGrid->Get_Mean());
		pRecord->Set_Value("MIN"         , pGrid->Get_ZMin());
		pRecord->Set_Value("MAX"         , pGrid->Get_ZMax());
		pRecord->Set_Value("RANGE"       , pGrid->Get_ZRange());
		pRecord->Set_Value("VAR"         , pGrid->Get_Variance());
		pRecord->Set_Value("STDDEV"      , pGrid->Get_StdDev());
		pRecord->Set_Value("STDDEVLO"    , pGrid->Get_Mean() - pGrid->Get_StdDev());
		pRecord->Set_Value("STDDEVHI"    , pGrid->Get_Mean() + pGrid->Get_StdDev());

		if( dRank > 0.0 && dRank < 100.0 )
		{
			pRecord->Set_Value("PCTL", pGrid->Get_Percentile(dRank));	// this is a time consuming operation
		}
	}

	if( dRank > 0.0 && dRank < 100.0 )
	{
		pTable->Set_Field_Name(pTable->Get_Field_Count() - 1, CSG_String::Format(SG_T("%s%02d"), _TL("PCTL"), (int)dRank));
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
