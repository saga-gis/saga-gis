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
#include "Grid_Normalise.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Normalise::CGrid_Normalise(void)
{
	Set_Name		(_TL("Grid Normalisation"));

	Set_Author		(SG_T("O.Conrad (c) 2003"));

	Set_Description	(_TW(
		"Normalise the values of a grid. "
		"Rescales all grid values to fall in the range 'Minimum' to 'Maximum', "
		"usually 0 to 1. "
	));

	Parameters.Add_Grid(
		NULL	, "INPUT"	,_TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "OUTPUT"	, _TL("Normalised Grid"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Range(
		NULL	, "RANGE"	, _TL("Target Range"),
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

	pGrid->Set_Name(CSG_String::Format(SG_T("%s (%s)"), pGrid->Get_Name(), _TL("Normalised")));

	//-----------------------------------------------------
	double		Minimum, Maximum, zMin, Stretch;

	Minimum	= Parameters("RANGE")->asRange()->Get_LoVal();
	Maximum	= Parameters("RANGE")->asRange()->Get_HiVal();
	zMin	= pGrid->Get_ZMin();
	Stretch	= (Maximum - Minimum) / pGrid->Get_ZRange();

	for(int y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !pGrid->is_NoData(x, y) )
			{
				pGrid->Set_Value(x, y, Minimum + Stretch * (pGrid->asDouble(x, y) - zMin));
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
	Set_Name		(_TL("Grid Standardisation"));

	Set_Author		(SG_T("O.Conrad (c) 2003"));

	Set_Description	(_TW(
		"Standardise the values of a grid. "
		"The standard score (z) is calculated as raw score (x) less "
		"arithmetic mean (m) divided by standard deviation (s).\n"
		"z = (x - m) * s"
	));

	Parameters.Add_Grid(
		NULL	, "INPUT"	,_TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "OUTPUT"	, _TL("Standardised Grid"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "STRETCH"	, _TL("Stretch Factor"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0
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

	pGrid->Set_Name(CSG_String::Format(SG_T("%s (%s)"), pGrid->Get_Name(), _TL("Standard Score")));

	//-----------------------------------------------------
	double	Mean	= pGrid->Get_Mean();
	double	Stretch	= Parameters("STRETCH")->asDouble() / pGrid->Get_StdDev();

	for(int y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
	{
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
