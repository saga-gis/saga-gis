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
#include "Grid_Completion.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Completion::CGrid_Completion(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Patching"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description	(_TW(
		"Fill gaps of a grid with data from another grid. "
	));


	//-----------------------------------------------------
	// 2. Standard in- and output...

	Parameters.Add_Grid(
		NULL	, "ORIGINAL"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "ADDITIONAL"		, _TL("Patch Grid"),
		_TL(""),
		PARAMETER_INPUT, false
	);

	Parameters.Add_Grid(
		NULL	, "COMPLETED"		, _TL("Completed Grid"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "RESAMPLING"		, _TL("Resampling"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|",
			_TL("Nearest Neighbour"),
			_TL("Bilinear Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 3
	);
}

//---------------------------------------------------------
CGrid_Completion::~CGrid_Completion(void)
{}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Completion::On_Execute(void)
{
	CSG_Grid	*pPatch	= Parameters("ADDITIONAL")->asGrid();
	CSG_Grid	*pGrid	= Parameters("COMPLETED" )->asGrid();

	if( !pGrid->is_Intersecting(pPatch->Get_Extent()) )
	{
		Error_Set(_TL("Nothing to do: there is no intersection with additional grid."));

		return( false );
	}

	if( pGrid != Parameters("ORIGINAL")->asGrid() )
	{
		Process_Set_Text(_TL("Copying original data..."));

		pGrid->Assign(Parameters("ORIGINAL")->asGrid());
	}

	TSG_Grid_Resampling	Resampling;

	switch( Parameters("RESAMPLING")->asInt() )
	{
	default:	Resampling	= GRID_RESAMPLING_NearestNeighbour;	break;
	case  1:	Resampling	= GRID_RESAMPLING_Bilinear;			break;
	case  2:	Resampling	= GRID_RESAMPLING_BicubicSpline;	break;
	case  3:	Resampling	= GRID_RESAMPLING_BSpline;			break;
	}

	Process_Set_Text(_TL("Data completion..."));

	int		x, y;
	double	xWorld, yWorld, Value;

	for(y=0, yWorld=Get_YMin(); y<Get_NY() && yWorld<=pPatch->Get_YMax() && Set_Progress(y, Get_NY()); y++, yWorld+=Get_Cellsize())
	{
		if( yWorld >= pPatch->Get_YMin() )
		{
			for(x=0, xWorld=Get_XMin(); x<Get_NX() && xWorld<=pPatch->Get_XMax(); x++, xWorld+=Get_Cellsize())
			{
				if( pGrid->is_NoData(x, y) && xWorld >= pPatch->Get_XMin() )
				{
					if( pPatch->Get_Value(xWorld, yWorld, Value, Resampling) )
					{
						pGrid->Set_Value(x, y, Value);
					}
				}
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
