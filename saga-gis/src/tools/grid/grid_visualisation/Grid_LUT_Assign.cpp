/**********************************************************
 * Version $Id: grid_lut_assign.cpp 2463 2015-04-02 09:02:26Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                  Grid_Visualisation                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  grid_lut_assign.cpp                  //
//                                                       //
//                 Copyright (C) 2015 by                 //
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
#include "Grid_LUT_Assign.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_LUT_Assign::CGrid_LUT_Assign(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Select Look-up Table for Grid Visualization"));

	Set_Author		("O.Conrad (c) 2015");

	Set_Description	(_TW(
		"Select a look-up table for visual classification of a grid. "
		"Useful in combination with tool chains. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "GRID"	, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table(
		NULL	, "LUT"		, _TL("Look-up Table"),
		_TL(""),
		PARAMETER_INPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_LUT_Assign::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid	*pGrid	= Parameters("GRID")->asGrid();

	CSG_Parameter	*pLUT	= DataObject_Get_Parameter(pGrid, "LUT");

	if( pLUT && pLUT->asTable() && pLUT->asTable()->Assign_Values(Parameters("LUT")->asTable()) )
	{
		DataObject_Set_Parameter(pGrid, pLUT);
		DataObject_Set_Parameter(pGrid, "COLORS_TYPE", 1);	// Color Classification Type: Lookup Table

		DataObject_Update(pGrid);

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
