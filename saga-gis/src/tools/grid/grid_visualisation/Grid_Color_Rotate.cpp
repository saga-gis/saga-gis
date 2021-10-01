
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
//                 Grid_Color_Rotate.cpp                 //
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
#include "Grid_Color_Rotate.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Color_Rotate::CGrid_Color_Rotate(void)
{
	Set_Name		(_TL("Color Palette Rotation"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description	(_TW(
		"The 'Color Palette Rotator' rotates the grids color palette. "
	));

	Parameters.Add_Grid  ("", "GRID"  , _TL("Grid"  ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Colors("", "COLORS", _TL("Colors"), _TL(""));
	Parameters.Add_Bool  ("", "DOWN"  , _TL("Down"  ), _TL(""), true);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Color_Rotate::On_Execute(void)
{
	CSG_Colors Colors = *Parameters("COLORS")->asColors();

	if( Colors.Get_Count() < 2 )
	{
		return( false );
	}

	CSG_Grid *pGrid = Parameters("GRID")->asGrid();

	bool bDown = Parameters("DOWN")->asBool();

	int n = Colors.Get_Count() - 1, d = bDown ? 1 : -1;

	do
	{
		long Color = Colors[bDown ? 0 : n];

		for(int i=0, j=bDown?0:n; i<n; i++, j+=d)
		{
			Colors[j] = Colors[j + d];
		}

		Colors[bDown ? n : 0] = Color;

		DataObject_Set_Colors(pGrid, Colors);
	}
	while( Process_Get_Okay() );

	return( SG_UI_Process_Set_Okay() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
