/**********************************************************
 * Version $Id$
 *********************************************************/

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
//                  Grid_Normalise.cpp                   //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Grid_Normalise.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Normalise::CGrid_Normalise(void)
{
	Set_Name		(_TL("Grid Normalization"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description	(_TW(
		"Normalise the values of a grid. "
		"Rescales all grid values to fall in the range 'Minimum' to 'Maximum', "
		"usually 0 to 1. "
	));

	Parameters.Add_Grid("",
		"INPUT"	,_TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"OUTPUT"	, _TL("Normalized Grid"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Range("",
		"RANGE"	, _TL("Target Range"),
		_TL(""),
		0.0, 1.0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Normalise::On_Execute(void)
{
	CSG_Grid	*pGrid	= Parameters("INPUT")->asGrid();

	if( pGrid->Get_StdDev() <= 0.0 )
	{
		return( false );
	}

	if( pGrid != Parameters("OUTPUT")->asGrid() )
	{
		pGrid	= Parameters("OUTPUT")->asGrid();
		pGrid	->Assign(Parameters("INPUT")->asGrid());
	}

	pGrid->Set_Name(CSG_String::Format("%s (%s)", pGrid->Get_Name(), _TL("Normalized")));

	//-----------------------------------------------------
	double		Minimum, Maximum, Offset, Scale;

	Minimum	= Parameters("RANGE")->asRange()->Get_LoVal();
	Maximum	= Parameters("RANGE")->asRange()->Get_HiVal();
	Offset	= pGrid->Get_Min();
	Scale	= (Maximum - Minimum) / pGrid->Get_Range();

	for(int y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !pGrid->is_NoData(x, y) )
			{
				pGrid->Set_Value(x, y, Minimum + Scale * (pGrid->asDouble(x, y) - Offset));
			}
		}
	}

	//-----------------------------------------------------
	if( pGrid == Parameters("INPUT")->asGrid() )
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
CGrid_Standardise::CGrid_Standardise(void)
{
	Set_Name		(_TL("Grid Standardization"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description	(_TW(
		"Standardize the values of a grid. "
		"The standard score (z) is calculated as raw score (x) less "
		"arithmetic mean (m) divided by standard deviation (s) and "
		"multiplied with the stretch factor (d):\n"
		"z = d * (x - m) / s"
	));

	Parameters.Add_Grid("",
		"INPUT"	,_TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"OUTPUT"	, _TL("Standardized Grid"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Double("",
		"STRETCH"	, _TL("Stretch Factor"),
		_TL(""),
		1.0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Standardise::On_Execute(void)
{
	CSG_Grid	*pGrid	= Parameters("INPUT")->asGrid();

	if( pGrid->Get_StdDev() <= 0.0 )
	{
		return( false );
	}

	if( pGrid != Parameters("OUTPUT")->asGrid() )
	{
		pGrid	= Parameters("OUTPUT")->asGrid();
		pGrid	->Assign(Parameters("INPUT")->asGrid());
	}

	pGrid->Set_Name(CSG_String::Format("%s (%s)", pGrid->Get_Name(), _TL("Standard Score")));

	//-----------------------------------------------------
	double	Mean	= pGrid->Get_Mean();
	double	Stretch	= Parameters("STRETCH")->asDouble() / pGrid->Get_StdDev();

	for(int y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( !pGrid->is_NoData(x, y) )
			{
				pGrid->Set_Value(x, y, Stretch * (pGrid->asDouble(x, y) - Mean));
			}
		}
	}

	//-----------------------------------------------------
	if( pGrid == Parameters("INPUT")->asGrid() )
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
