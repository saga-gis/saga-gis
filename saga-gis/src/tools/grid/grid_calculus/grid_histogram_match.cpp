
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     Grid_Calculus                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                grid_histogram_match.cpp               //
//                                                       //
//                 Copyright (C) 2019 by                 //
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
#include "grid_histogram_match.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Histogram_Match::CGrid_Histogram_Match(void)
{
	Set_Name		(_TL("Histogram Matching"));

	Set_Author		("O.Conrad (c) 2019");

	Set_Description	(_TW(
		"This tool alters the values of a grid so that its value "
		"distribution (its histogram), matches that of a reference grid. "
		"The first method simply uses arithmetic mean and standard "
		"deviation for adjustment, which usually is sufficient for normal "
		"distributed values. The second method performs a more precise "
		"adjustment based on the grids' histograms. "
	));

	Parameters.Add_Grid(
		"", "GRID"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "MATCHED"	, _TL("Adjusted Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		"", "REFERENCE"	, _TL("Reference Grid"),
		_TL(""),
		PARAMETER_INPUT, false
	);

	Parameters.Add_Choice(
		"", "METHOD"	, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("standard deviation"),
			_TL("histogram")
		), 1
	);

	Parameters.Add_Int(
		"", "NCLASSES"	, _TL("Histogramm Classes"),
		_TL("Number of histogram classes for internal use."),
		10, 100, true
	);

	Parameters.Add_Int(
		"", "MAXSAMPLES", _TL("Maximum Sample Size"),
		_TL("If set to zero all data will be used to build the histograms."),
		1000000, 0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Histogram_Match::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("NCLASSES"  , pParameter->asInt() == 1);
		pParameters->Set_Enabled("MAXSAMPLES", pParameter->asInt() == 1);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Histogram_Match::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid	*pGrid	= Parameters("GRID")->asGrid();

	if( Parameters("MATCHED")->asGrid() && Parameters("MATCHED")->asGrid() != pGrid )
	{
		CSG_Grid	*pResult	= Parameters("MATCHED")->asGrid();

		pResult->Create(*pGrid);

		pResult->Fmt_Name("%s [%s]", pGrid->Get_Name(), _TL("histogram match"));

		pGrid	= pResult;
	}

	//-----------------------------------------------------
	CSG_Grid	*pReference	= Parameters("REFERENCE")->asGrid();

	CSG_Simple_Statistics Statistics[2]; CSG_Histogram Histogram[2];

	int	Method	= Parameters("METHOD")->asInt();

	switch( Method )
	{
	case  0:
		Statistics[0]	= pReference->Get_Statistics();

		if( Statistics[0].Get_StdDev() <= 0.0 )
		{
			Error_Fmt("%s [%s]", _TL("no variance in data set"), pReference->Get_Name());

			return( false );
		}

		Statistics[1]	= pGrid->Get_Statistics();

		if( Statistics[1].Get_StdDev() <= 0.0 )
		{
			Error_Fmt("%s [%s]", _TL("no variance in data set"), pGrid->Get_Name());

			return( false );
		}
		break;

	default:
		if( !Histogram[0].Create(Parameters("NCLASSES")->asInt(),
			pReference->Get_Min(), pReference->Get_Max(), pReference, (size_t)Parameters("MAXSAMPLES")->asInt()) )
		{
			Error_Fmt("%s [%s]", _TL("failed to create histogram"), pReference->Get_Name());

			return( false );
		}

		if( !Histogram[1].Create(Parameters("NCLASSES")->asInt(),
			pGrid->Get_Min(), pGrid->Get_Max(), pGrid, (size_t)Parameters("MAXSAMPLES")->asInt()) )
		{
			Error_Fmt("%s [%s]", _TL("failed to create histogram"), pGrid->Get_Name());

			return( false );
		}
		break;
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#ifndef _DEBUG
		#pragma omp parallel for
		#endif
		for(int x=0; x<Get_NX(); x++)
		{
			if( !pGrid->is_NoData(x, y) )
			{
				double	v	= pGrid->asDouble(x, y);

				switch( Method )
				{
				case  0:
					v	= Statistics[0].Get_Mean() + (v - Statistics[1].Get_Mean()) * Statistics[0].Get_StdDev() / Statistics[1].Get_StdDev();
					break;

				default:
					v	= Histogram[0].Get_Quantile(Histogram[1].Get_Quantile_Value(v));
					break;
				}

				pGrid->Set_Value(x, y, v);
			}
		}
	}

	//-----------------------------------------------------
	if( pGrid != Parameters("MATCHED")->asGrid() )
	{
		DataObject_Update(pGrid);	// only declared output data is updated automatically by the framework
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
