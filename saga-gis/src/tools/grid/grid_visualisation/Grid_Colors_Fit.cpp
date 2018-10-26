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
//                  Grid_Visualisation                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  Grid_Colors_Fit.cpp                  //
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
#include "Grid_Colors_Fit.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Colors_Fit::CGrid_Colors_Fit(void)
{
	Set_Name		(_TL("Fit Color Palette to Grid Values"));

	Set_Author		("O.Conrad (c) 2005");

	Set_Description	(_TW(
		""
	));

	Parameters.Add_Grid("",
		"GRID"	, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Int("",
		"COUNT"	, _TL("Number of Colors"),
		_TL(""),
		100, 2, true
	);

	Parameters.Add_Choice("",
		"SCALE"	, _TL("Scale"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("Grid range"),
			_TL("User defined range")
		), 0
	);

	Parameters.Add_Range("",
		"RANGE"	, _TL("User defined range"),
		_TL(""),
		0.0, 1.0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Colors_Fit::On_Execute(void)
{
	CSG_Grid	*pGrid	= Parameters("GRID")->asGrid();

	//-----------------------------------------------------
	double	zMin, zRange;

	switch( Parameters("SCALE")->asInt() )
	{
	default:
		zMin	= pGrid->Get_Min();
		zRange	= pGrid->Get_Max() - zMin;
		break;

	case  1:
		zMin	= Parameters("RANGE")->asRange()->Get_Min();
		zRange	= Parameters("RANGE")->asRange()->Get_Max() - zMin;
		break;
	}

	if( zRange == 0.0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Colors	Colors_Old, Colors_New;

	Colors_New.Set_Count(Parameters("COUNT")->asInt());

	DataObject_Get_Colors(pGrid, Colors_Old);

	if( Colors_Old.Get_Count() < 2 )
	{
		return( false );
	}

	//-----------------------------------------------------
	double	bZ, aZ	= 0.0;
	long	bC, aC	= Colors_Old.Get_Color(0);

	for(int iColor=1; iColor<Colors_Old.Get_Count()-1; iColor++)
	{
		bZ	= aZ;
		bC	= aC;
		aZ	= (pGrid->Get_Quantile(iColor / Colors_Old.Get_Count()) - zMin) / zRange;
		aC	= Colors_Old.Get_Color(iColor);
		_Set_Colors(Colors_New, bZ, bC, aZ, aC);
	}

	bZ	= aZ;
	bC	= aC;
	aZ	= 1.0;
	aC	= Colors_Old.Get_Color(Colors_Old.Get_Count() - 1);
	_Set_Colors(Colors_New, bZ, bC, aZ, aC);

	DataObject_Set_Colors(pGrid, Colors_New);
	DataObject_Update    (pGrid, zMin, zMin + zRange);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGrid_Colors_Fit::_Set_Colors(CSG_Colors &Colors, double pos_a, long color_a, double pos_b, long color_b)
{
	int a = (int)(pos_a * Colors.Get_Count()); if( a < 0 ) a = 0; else if( a >= Colors.Get_Count() ) a = Colors.Get_Count() - 1;
	int b = (int)(pos_b * Colors.Get_Count()); if( b < 0 ) b = 0; else if( b >= Colors.Get_Count() ) b = Colors.Get_Count() - 1;

	Colors.Set_Ramp(color_a, color_b, a, b);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
