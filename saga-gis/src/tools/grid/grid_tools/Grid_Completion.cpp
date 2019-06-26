
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       Tutorial                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  Grid_Completion.cpp                  //
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
#include "Grid_Completion.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Completion::CGrid_Completion(void)
{
	Set_Name		(_TL("Patching"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description	(_TW(
		"Fill gaps of a grid with data from another grid. "
		"If the target is not set, the changes will be stored to the original grid. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		"", "ORIGINAL"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "COMPLETED"		, _TL("Patched Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		"", "ADDITIONAL"	, _TL("Patch Grid"),
		_TL(""),
		PARAMETER_INPUT, false
	);

	Parameters.Add_Choice(
		"", "RESAMPLING"	, _TL("Resampling"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("Nearest Neighbour"),
			_TL("Bilinear Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 3
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Completion::On_Execute(void)
{
	CSG_Grid	*pPatch	= Parameters("ADDITIONAL")->asGrid();
	CSG_Grid	*pGrid	= Parameters("ORIGINAL"  )->asGrid();

	if( !pGrid->is_Intersecting(pPatch->Get_Extent()) )
	{
		Error_Set(_TL("Nothing to do: there is no intersection with additional grid."));

		return( false );
	}

	//-----------------------------------------------------
	if( pGrid != Parameters("COMPLETED")->asGrid() && Parameters("COMPLETED")->asGrid() )
	{
		Process_Set_Text(_TL("Copying original data..."));

		CSG_Grid	*pResult	= Parameters("COMPLETED")->asGrid();

		pResult->Create(*pGrid);

		pResult->Fmt_Name("%s [%s]", pGrid->Get_Name(), _TL("Patched"));

		DataObject_Set_Parameters(pResult, pGrid);

		pGrid	= pResult;
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
	Process_Set_Text(_TL("Data completion..."));

	for(int y=0; y<Get_NY() && Set_Progress(y, Get_NY()); y++)
	{
		double	yWorld	= Get_YMin() + y * Get_Cellsize();

		if( pPatch->Get_YMin() <= yWorld && yWorld <= pPatch->Get_YMax() )
		{
			for(int x=0; x<Get_NX(); x++)
			{
				if( pGrid->is_NoData(x, y) )
				{
					double	Value, xWorld	= Get_XMin() + x * Get_Cellsize();

				//	if( pPatch->Get_XMin() <= xWorld && xWorld <= pPatch->Get_XMax() )
					if( pPatch->Get_Value(xWorld, yWorld, Value, Resampling) )
					{
						pGrid->Set_Value(x, y, Value);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	if( pGrid == Parameters("ORIGINAL")->asGrid() )
	{
		DataObject_Update(pGrid);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
